// crypto_core.h - шифрування для РГР з ТЗІ: комбінований шифр (афінний + перестановка) і RSA
// Юхно О.В., 401-ТН
// тут тільки C++, без .NET, щоб можна було окремо зібрати й перевірити
#pragma once
#ifndef CRYPTO_CORE_H
#define CRYPTO_CORE_H

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iomanip>

namespace crypto {

	// розмір блоку для перестановки
	const int BLOCK_SIZE = 16;

	// працюємо з байтами (0..255), а не з символами, тому підходить будь-який файл
	const int ALPHABET = 256;


	// ---------- теорія чисел ----------

	// розширений алгоритм Евкліда: a*x + b*y = НСД(a, b)
	inline long long egcd(long long a, long long b, long long& x, long long& y) {
		if (b == 0) {
			x = 1;
			y = 0;
			return a;
		}
		long long x1, y1;
		long long g = egcd(b, a % b, x1, y1);
		x = y1;
		y = x1 - (a / b) * y1;
		return g;
	}

	// обернений за модулем: a * a^-1 = 1 (mod m), або -1, якщо його нема
	inline long long modInverse(long long a, long long m) {
		if (m <= 0) return -1;
		a %= m;
		if (a < 0) a += m;
		long long x, y;
		long long g = egcd(a, m, x, y);
		if (g != 1) return -1;              // оберненого не існує
		x %= m;
		if (x < 0) x += m;
		return x;
	}

	// швидке піднесення до степеня за модулем (квадрат-множення)
	// без нього RSA з великими ключами рахувався б вічність
	inline long long modPow(long long base, long long exp, long long mod) {
		if (mod <= 1) return 0;
		long long result = 1;
		base %= mod;
		if (base < 0) base += mod;
		while (exp > 0) {
			if (exp & 1) result = (result * base) % mod;
			base = (base * base) % mod;
			exp >>= 1;
		}
		return result;
	}


	// ---------- ключі ----------
	// усе з одного ключа користувача: FNV-1a -> зерно -> xorshift32 -> a, b, перестановка

	// FNV-1a: ключ будь-якої довжини -> 32-бітне зерно
	inline unsigned int fnv1a32(const std::string& key) {
		unsigned int h = 2166136261u;                 // offset basis
		for (size_t i = 0; i < key.size(); ++i) {
			h ^= static_cast<unsigned char>(key[i]);
			h *= 16777619u;                           // FNV prime
		}
		return h;
	}

	// xorshift32 - з нього беруться всі підключі
	struct KeyStream {
		unsigned int state;

		explicit KeyStream(unsigned int seed) {
			// якщо зерно 0, xorshift видаватиме самі нулі
			state = (seed != 0u) ? seed : 0x9E3779B9u;
		}

		unsigned int next() {
			state ^= state << 13;
			state ^= state >> 17;
			state ^= state << 5;
			return state;
		}

		// число від 0 до n-1
		unsigned int below(unsigned int n) {
			return (n == 0u) ? 0u : (next() % n);
		}
	};

	// всі підключі, які виходять з ключа користувача
	struct KeySchedule {
		std::string      userKey;   // ключ користувача (UTF-8)
		unsigned int     seed;      // зерно з FNV-1a
		int              a;         // множник афінного шифру
		int              aInv;      // a^-1 mod 256
		int              b;         // зсув афінного шифру
		std::vector<int> perm;      // перестановка блоку
		std::vector<int> invPerm;   // обернена перестановка
		bool             valid;     // false, якщо ключ порожній
	};

	inline KeySchedule deriveKeys(const std::string& userKey) {
		KeySchedule ks;
		ks.userKey = userKey;
		ks.valid = !userKey.empty();
		ks.seed = fnv1a32(userKey);

		KeyStream rnd(ks.seed);

		// a має бути непарним, інакше не буде оберненого за mod 256
		// a = 1 пропускаємо, бо це вже просто зсув
		do {
			ks.a = static_cast<int>(rnd.next() % ALPHABET) | 1;
		} while (ks.a == 1);

		ks.b = static_cast<int>(rnd.next() % ALPHABET);
		ks.aInv = static_cast<int>(modInverse(ks.a, ALPHABET));

		// таблиця перестановки (Фішер-Йєтс)
		ks.perm.resize(BLOCK_SIZE);
		for (int i = 0; i < BLOCK_SIZE; ++i) ks.perm[i] = i;
		for (int i = BLOCK_SIZE - 1; i > 0; --i) {
			int j = static_cast<int>(rnd.below(static_cast<unsigned int>(i + 1)));
			int t = ks.perm[i];
			ks.perm[i] = ks.perm[j];
			ks.perm[j] = t;
		}

		ks.invPerm.resize(BLOCK_SIZE);
		for (int i = 0; i < BLOCK_SIZE; ++i) ks.invPerm[ks.perm[i]] = i;

		return ks;
	}


	// метод 1: афінний шифр, C = (a*P + b) mod 256
	inline unsigned char affineEncryptByte(unsigned char p, const KeySchedule& ks) {
		return static_cast<unsigned char>((ks.a * static_cast<int>(p) + ks.b) % ALPHABET);
	}

	inline unsigned char affineDecryptByte(unsigned char c, const KeySchedule& ks) {
		int v = (static_cast<int>(c) - ks.b) % ALPHABET;
		if (v < 0) v += ALPHABET;
		return static_cast<unsigned char>((ks.aInv * v) % ALPHABET);
	}


	// метод 2: перестановка байтів у блоці
	// останній блок буває коротший за 16 байт, доповнювати його не можна
	// (зміниться розмір файлу), тому з таблиці беремо тільки числа < len
	inline std::vector<int> blockPerm(const KeySchedule& ks, int len) {
		if (len >= BLOCK_SIZE) return ks.perm;
		std::vector<int> q;
		q.reserve(static_cast<size_t>(len));
		for (int i = 0; i < BLOCK_SIZE; ++i) {
			if (ks.perm[i] < len) q.push_back(ks.perm[i]);
		}
		return q;
	}

	// байт з позиції i йде на позицію q[i]
	inline void permuteBlock(unsigned char* buf, int len, const std::vector<int>& q) {
		std::vector<unsigned char> tmp(buf, buf + len);
		for (int i = 0; i < len; ++i) buf[q[i]] = tmp[i];
	}

	// зворотна перестановка
	inline void unpermuteBlock(unsigned char* buf, int len, const std::vector<int>& q) {
		std::vector<unsigned char> tmp(buf, buf + len);
		for (int i = 0; i < len; ++i) buf[i] = tmp[q[i]];
	}


	// спочатку метод 1, потім метод 2, і тільки тоді запис у файл
	inline void encryptBlock(unsigned char* buf, int len, const KeySchedule& ks) {
		for (int i = 0; i < len; ++i)                 // метод 1
			buf[i] = affineEncryptByte(buf[i], ks);
		permuteBlock(buf, len, blockPerm(ks, len));   // метод 2
	}

	// розшифрування - у зворотному порядку
	inline void decryptBlock(unsigned char* buf, int len, const KeySchedule& ks) {
		unpermuteBlock(buf, len, blockPerm(ks, len)); // метод 2 (обернений)
		for (int i = 0; i < len; ++i)                 // метод 1 (обернений)
			buf[i] = affineDecryptByte(buf[i], ks);
	}


	// ---------- допоміжне: hex і опис ключів ----------

	inline std::wstring bytesToHex(const unsigned char* data, int len) {
		std::wostringstream os;
		os << std::hex << std::uppercase << std::setfill(L'0');
		for (int i = 0; i < len; ++i) {
			if (i) os << L' ';
			os << std::setw(2) << static_cast<int>(data[i]);
		}
		return os.str();
	}

	inline std::wstring bytesToHex(const std::string& data) {
		if (data.empty()) return std::wstring();
		return bytesToHex(reinterpret_cast<const unsigned char*>(data.data()),
			static_cast<int>(data.size()));
	}

	// hex-рядок -> байти (пробіли й переноси пропускаємо)
	inline std::string hexToBytes(const std::wstring& hex, bool* ok) {
		std::string out;
		int hi = -1;
		for (size_t i = 0; i < hex.size(); ++i) {
			wchar_t c = hex[i];
			if (c == L' ' || c == L'\n' || c == L'\r' || c == L'\t') continue;
			int v;
			if (c >= L'0' && c <= L'9')      v = c - L'0';
			else if (c >= L'a' && c <= L'f') v = c - L'a' + 10;
			else if (c >= L'A' && c <= L'F') v = c - L'A' + 10;
			else { if (ok) *ok = false; return std::string(); }

			if (hi < 0) hi = v;
			else { out.push_back(static_cast<char>(hi * 16 + v)); hi = -1; }
		}
		if (hi >= 0) { if (ok) *ok = false; return std::string(); }  // непарна кількість цифр
		if (ok) *ok = true;
		return out;
	}

	// опис підключів для вікна програми
	inline std::wstring describeKeys(const KeySchedule& ks) {
		std::wostringstream os;
		os << L"Ключ користувача .......... \"";
		// недруковані символи замінюємо крапкою
		for (size_t i = 0; i < ks.userKey.size(); ++i) {
			unsigned char c = static_cast<unsigned char>(ks.userKey[i]);
			os << (c >= 32 && c < 127 ? static_cast<wchar_t>(c) : L'.');
		}
		os << L"\"\n";
		os << L"Зерно (FNV-1a) ............ " << ks.seed
			<< L" (0x" << std::hex << std::uppercase << ks.seed << std::dec << L")\n";
		os << L"Метод 1, аналітичні перетворення:  C = (" << ks.a
			<< L" * P + " << ks.b << L") mod 256\n";
		os << L"Обернений множник a^-1 .... " << ks.aInv
			<< L"   (перевірка: " << ks.a << L" * " << ks.aInv
			<< L" mod 256 = " << ((ks.a * ks.aInv) % ALPHABET) << L")\n";
		os << L"Метод 2, проста перестановка блоку з " << BLOCK_SIZE << L" байтів:\n";
		os << L"   i    -> ";
		for (int i = 0; i < BLOCK_SIZE; ++i) os << std::setw(3) << i;
		os << L"\n   P(i) -> ";
		for (int i = 0; i < BLOCK_SIZE; ++i) os << std::setw(3) << ks.perm[i];
		os << L"\n";
		return os.str();
	}


	// ---------- шифрування тексту (вкладка "Текст") ----------

	inline std::string processBytes(const std::string& in, const KeySchedule& ks,
		bool encrypt, std::wstring* trace) {
		std::string out = in;
		int blocks = 0;

		for (size_t off = 0; off < out.size(); off += BLOCK_SIZE) {
			int len = static_cast<int>(out.size() - off);
			if (len > BLOCK_SIZE) len = BLOCK_SIZE;
			unsigned char* buf = reinterpret_cast<unsigned char*>(&out[off]);

			if (trace && blocks < 2) {
				std::wostringstream os;
				os << L"\n--- Блок " << (blocks + 1) << L" (" << len << L" Б) ---\n";
				os << L"  вхід .............. " << bytesToHex(buf, len) << L"\n";
				*trace += os.str();
			}

			if (encrypt) {
				// метод 1
				for (int i = 0; i < len; ++i) buf[i] = affineEncryptByte(buf[i], ks);
				if (trace && blocks < 2)
					*trace += L"  після методу 1 .... " + bytesToHex(buf, len) + L"\n";
				// метод 2
				permuteBlock(buf, len, blockPerm(ks, len));
				if (trace && blocks < 2)
					*trace += L"  після методу 2 .... " + bytesToHex(buf, len) + L"\n";
			}
			else {
				unpermuteBlock(buf, len, blockPerm(ks, len));
				if (trace && blocks < 2)
					*trace += L"  знято перестановку  " + bytesToHex(buf, len) + L"\n";
				for (int i = 0; i < len; ++i) buf[i] = affineDecryptByte(buf[i], ks);
				if (trace && blocks < 2)
					*trace += L"  знято афінний шифр  " + bytesToHex(buf, len) + L"\n";
			}
			++blocks;
		}

		if (trace && blocks > 2) {
			std::wostringstream os;
			os << L"\n... решта блоків (" << (blocks - 2)
				<< L") оброблена так само.\n";
			*trace += os.str();
		}
		return out;
	}


	// ---------- шифрування файлу ----------
	// читаємо по 16 байт, шифруємо і одразу пишемо, тому розмір файлу не міняється

	struct FileReport {
		bool               ok;
		std::wstring       message;
		std::wstring       trace;
		unsigned long long bytes;

		FileReport() : ok(false), bytes(0) {}
	};

	inline FileReport processFile(const std::wstring& inPath,
		const std::wstring& outPath,
		const KeySchedule& ks,
		bool encrypt) {
		FileReport rep;

		if (!ks.valid) {
			rep.message = L"Помилка: не задано ключ користувача.";
			return rep;
		}
		if (inPath.empty() || outPath.empty()) {
			rep.message = L"Помилка: не вказано вхідний або вихідний файл.";
			return rep;
		}
		if (inPath == outPath) {
			rep.message = L"Помилка: вхідний і вихідний файли збігаються.";
			return rep;
		}

		std::ifstream fin(inPath.c_str(), std::ios::binary);
		if (!fin) {
			rep.message = L"Помилка: не вдалося відкрити вхідний файл.";
			return rep;
		}
		std::ofstream fout(outPath.c_str(), std::ios::binary | std::ios::trunc);
		if (!fout) {
			rep.message = L"Помилка: не вдалося створити вихідний файл.";
			return rep;
		}

		unsigned char buf[BLOCK_SIZE];
		int blocks = 0;

		while (fin) {
			fin.read(reinterpret_cast<char*>(buf), BLOCK_SIZE);
			int len = static_cast<int>(fin.gcount());
			if (len <= 0) break;

			// перші два блоки детально показуємо в журналі
			bool show = (blocks < 2);
			if (show) {
				std::wostringstream os;
				os << L"\n--- Блок " << (blocks + 1) << L" (" << len << L" Б) ---\n";
				os << L"  з вхідного файлу .. " << bytesToHex(buf, len) << L"\n";
				rep.trace += os.str();
			}

			if (encrypt) {
				for (int i = 0; i < len; ++i) buf[i] = affineEncryptByte(buf[i], ks);
				if (show) rep.trace += L"  після методу 1 .... " + bytesToHex(buf, len) + L"\n";
				permuteBlock(buf, len, blockPerm(ks, len));
				if (show) rep.trace += L"  після методу 2 .... " + bytesToHex(buf, len) + L"\n";
			}
			else {
				unpermuteBlock(buf, len, blockPerm(ks, len));
				if (show) rep.trace += L"  знято перестановку  " + bytesToHex(buf, len) + L"\n";
				for (int i = 0; i < len; ++i) buf[i] = affineDecryptByte(buf[i], ks);
				if (show) rep.trace += L"  знято афінний шифр  " + bytesToHex(buf, len) + L"\n";
			}

			if (show) rep.trace += L"  -> записано у вихідний файл\n";

			fout.write(reinterpret_cast<const char*>(buf), len);
			if (!fout) {
				rep.message = L"Помилка: збій під час запису вихідного файлу.";
				return rep;
			}

			rep.bytes += static_cast<unsigned long long>(len);
			++blocks;
		}

		fout.close();

		if (blocks > 2) {
			std::wostringstream os;
			os << L"\n... решта блоків (" << (blocks - 2)
				<< L") оброблена за тим самим конвеєром.\n";
			rep.trace += os.str();
		}

		std::wostringstream os;
		os << (encrypt ? L"Файл зашифровано. " : L"Файл розшифровано. ")
			<< L"Оброблено " << rep.bytes << L" байт(ів), блоків: " << blocks << L".";
		rep.message = os.str();
		rep.ok = true;
		return rep;
	}


	// ---------- RSA ----------
	// дві пари ключів - своя і чужа
	// захист:  S = M^d_свій mod n_свій,   C = S^e_чужий mod n_чужий
	// зняття:  S = C^d_чужий mod n_чужий, M = S^e_свій mod n_свій
	// працює тільки якщо n_свій < n_чужий

	struct RsaKeyPair {
		std::wstring owner;
		long long p, q, n, phi, e, d;
		bool valid;

		RsaKeyPair() : p(0), q(0), n(0), phi(0), e(0), d(0), valid(false) {}
	};

	inline RsaKeyPair makeRsa(const std::wstring& owner, long long p, long long q, long long e) {
		RsaKeyPair k;
		k.owner = owner;
		k.p = p;
		k.q = q;
		k.n = p * q;
		k.phi = (p - 1) * (q - 1);
		k.e = e;
		k.d = modInverse(e, k.phi);
		// ключ годиться, якщо d існує і n > 255 (щоб влізли всі байти)
		k.valid = (k.d > 0) && (k.n > 255);
		return k;
	}

	inline std::wstring describeRsa(const RsaKeyPair& k) {
		std::wostringstream os;
		os << k.owner << L":\n";
		os << L"   p = " << k.p << L", q = " << k.q
			<< L"   ->   n = p*q = " << k.n
			<< L",  f(n) = (p-1)(q-1) = " << k.phi << L"\n";
		os << L"   відкритий ключ (e, n) = (" << k.e << L", " << k.n << L")\n";
		os << L"   закритий  ключ (d, n) = (" << k.d << L", " << k.n << L")\n";
		os << L"   перевірка: e*d mod f(n) = "
			<< ((k.e * k.d) % k.phi) << L"\n";
		if (!k.valid) os << L"   УВАГА: ключ некоректний!\n";
		return os.str();
	}

	// свій закритий ключ, потім чужий відкритий
	// спрацює тільки якщо n_свій < n_чужий
	inline std::wstring rsaProtect(const std::string& message,
		const RsaKeyPair& own,
		const RsaKeyPair& other,
		std::wstring* trace,
		bool* ok) {
		if (ok) *ok = false;

		if (!own.valid || !other.valid) {
			if (trace) *trace += L"Помилка: некоректні параметри ключів RSA.\n";
			return std::wstring();
		}
		if (own.n >= other.n) {
			if (trace) *trace += L"Помилка: потрібно n_свій < n_чужий, "
				L"інакше ланцюжок перетворень незворотний.\n";
			return std::wstring();
		}
		if (message.empty()) {
			if (trace) *trace += L"Помилка: порожнє повідомлення.\n";
			return std::wstring();
		}

		std::wostringstream out;
		std::wostringstream t1, t2;

		for (size_t i = 0; i < message.size(); ++i) {
			long long m = static_cast<unsigned char>(message[i]);
			long long s = modPow(m, own.d, own.n);        // підпис своїм d
			long long c = modPow(s, other.e, other.n);    // шифр чужим e

			if (i) { out << L' '; t1 << L' '; t2 << L' '; }
			t1 << s;
			t2 << c;
			out << c;
		}

		if (trace) {
			*trace += L"Байти повідомлення (hex):\n  " + bytesToHex(message) + L"\n";
			*trace += L"\nКрок 1. Підпис своїм ЗАКРИТИМ ключом  S = M^"
				+ std::to_wstring(own.d) + L" mod " + std::to_wstring(own.n) + L":\n  "
				+ t1.str() + L"\n";
			*trace += L"\nКрок 2. Шифрування чужим ВІДКРИТИМ ключом  C = S^"
				+ std::to_wstring(other.e) + L" mod " + std::to_wstring(other.n) + L":\n  "
				+ t2.str() + L"\n";
		}

		if (ok) *ok = true;
		return out.str();
	}

	// зняття захисту: чужий закритий ключ, потім свій відкритий
	inline std::string rsaUnprotect(const std::wstring& cipher,
		const RsaKeyPair& own,
		const RsaKeyPair& other,
		std::wstring* trace,
		bool* ok) {
		if (ok) *ok = false;
		std::string result;

		if (!own.valid || !other.valid) {
			if (trace) *trace += L"Помилка: некоректні параметри ключів RSA.\n";
			return result;
		}

		// розбираємо числа з рядка
		std::vector<long long> nums;
		std::wistringstream is(cipher);
		std::wstring word;
		while (is >> word) {
			bool digits = !word.empty();
			for (size_t i = 0; i < word.size(); ++i)
				if (word[i] < L'0' || word[i] > L'9') digits = false;
			if (!digits) {
				if (trace) *trace += L"Помилка: очікувався рядок цілих чисел, "
					L"розділених пробілами. Хибний елемент: \"" + word + L"\".\n";
				return result;
			}
			long long v = 0;
			for (size_t i = 0; i < word.size(); ++i) v = v * 10 + (word[i] - L'0');
			if (v >= other.n) {
				if (trace) *trace += L"Помилка: число " + std::to_wstring(v)
					+ L" не менше за модуль n = " + std::to_wstring(other.n) + L".\n";
				return result;
			}
			nums.push_back(v);
		}

		if (nums.empty()) {
			if (trace) *trace += L"Помилка: порожній шифротекст.\n";
			return result;
		}

		std::wostringstream t1, t2;
		for (size_t i = 0; i < nums.size(); ++i) {
			long long s = modPow(nums[i], other.d, other.n);   // знімаємо шифр чужим d
			long long m = modPow(s, own.e, own.n);             // перевіряємо підпис своїм e
			if (i) { t1 << L' '; t2 << L' '; }
			t1 << s;
			t2 << m;
			result.push_back(static_cast<char>(m & 0xFF));
		}

		if (trace) {
			*trace += L"Крок 1. Розшифрування чужим ЗАКРИТИМ ключом  S = C^"
				+ std::to_wstring(other.d) + L" mod " + std::to_wstring(other.n) + L":\n  "
				+ t1.str() + L"\n";
			*trace += L"\nКрок 2. Перевірка підпису своїм ВІДКРИТИМ ключом  M = S^"
				+ std::to_wstring(own.e) + L" mod " + std::to_wstring(own.n) + L":\n  "
				+ t2.str() + L"\n";
			*trace += L"\nВідновлені байти (hex):\n  " + bytesToHex(result) + L"\n";
		}

		if (ok) *ok = true;
		return result;
	}


	// ---------- самоперевірка при запуску ----------

	inline std::wstring selfTest(const RsaKeyPair& own, const RsaKeyPair& other) {
		std::wostringstream os;
		os << L"САМОПЕРЕВІРКА\n";

		// 1. комбінований шифр: блоки довжиною від 0 до 40 байт
		KeySchedule ks = deriveKeys("test-key-2026");
		bool combinedOk = true;
		for (int len = 0; len <= 40 && combinedOk; ++len) {
			std::string data;
			for (int i = 0; i < len; ++i)
				data.push_back(static_cast<char>((i * 37 + len * 11) & 0xFF));
			std::string enc = processBytes(data, ks, true, 0);
			std::string dec = processBytes(enc, ks, false, 0);
			if (dec != data || enc.size() != data.size()) combinedOk = false;
		}

		// 2. всі 256 значень байта
		std::string all;
		for (int i = 0; i < 256; ++i) all.push_back(static_cast<char>(i));
		std::string allEnc = processBytes(all, ks, true, 0);
		std::string allDec = processBytes(allEnc, ks, false, 0);
		if (allDec != all) combinedOk = false;

		os << L"  Комбінований шифр (довжини 0..40 Б, усі 256 значень байта) .. "
			<< (combinedOk ? L"ПРОЙДЕНО" : L"ПОМИЛКА") << L"\n";

		// 3. RSA туди й назад для всіх 256 байтів
		bool rsaOk = own.valid && other.valid && (own.n < other.n);
		if (rsaOk) {
			for (int i = 0; i < 256 && rsaOk; ++i) {
				long long s = modPow(i, own.d, own.n);
				long long c = modPow(s, other.e, other.n);
				long long s2 = modPow(c, other.d, other.n);
				long long m = modPow(s2, own.e, own.n);
				if (m != i) rsaOk = false;
			}
		}
		os << L"  RSA, ланцюжок свій закритий -> чужий відкритий і назад "
			<< L"(усі 256 значень) .. " << (rsaOk ? L"ПРОЙДЕНО" : L"ПОМИЛКА") << L"\n";

		return os.str();
	}

} // namespace crypto

#endif // CRYPTO_CORE_H
