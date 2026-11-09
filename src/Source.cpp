// Source.cpp - реалізація func
// Юхно О.В., 401-ТН
#include "func.h"
#include <cstdio>

// ключі RSA: своя і чужа пара
// числа маленькі навмисно, щоб у звіті було видно всі обчислення
// n_свій < n_чужий, інакше ланцюжок не розшифрується
namespace {
	const long long OWN_P = 61, OWN_Q = 53, OWN_E = 17;   // n = 3233
	const long long OTH_P = 71, OTH_Q = 47, OTH_E = 13;   // n = 3337
}

func::func() {
	ok = false;
	ks = crypto::deriveKeys("");                 // порожній ключ = недійсний
	own = crypto::makeRsa(L"Свої ключі (Юхно О.В.)", OWN_P, OWN_Q, OWN_E);
	other = crypto::makeRsa(L"Чужі ключі (кореспондент)", OTH_P, OTH_Q, OTH_E);
}

func::~func() {}

// ---------- рядки ----------

std::string func::ToBytes(String^ s) {
	std::string out;
	if (s == nullptr) return out;
	array<Byte>^ bytes = System::Text::Encoding::UTF8->GetBytes(s);
	out.resize(static_cast<size_t>(bytes->Length));
	for (int i = 0; i < bytes->Length; ++i)
		out[static_cast<size_t>(i)] = static_cast<char>(bytes[i]);
	return out;
}

String^ func::FromBytes(const std::string& s) {
	array<Byte>^ bytes = gcnew array<Byte>(static_cast<int>(s.size()));
	for (size_t i = 0; i < s.size(); ++i)
		bytes[static_cast<int>(i)] = static_cast<Byte>(static_cast<unsigned char>(s[i]));
	return System::Text::Encoding::UTF8->GetString(bytes);
}

String^ func::W(const std::wstring& w) {
	// у TextBox переноси рядків мають бути \r\n
	return (gcnew String(w.c_str()))->Replace(L"\n", L"\r\n");
}

std::wstring func::ToW(String^ s) {
	if (s == nullptr) return std::wstring();
	return msclr::interop::marshal_as<std::wstring>(s);
}

std::string func::SystemStringToStdString(String^ sysString) {
	return msclr::interop::marshal_as<std::string>(sysString);
}

String^ func::StdStringToSystemString(const std::string& stdString) {
	return msclr::interop::marshal_as<String^>(stdString);
}

// ---------- ключі ----------

void func::SetUserKey(String^ key) {
	ks = crypto::deriveKeys(ToBytes(key));
}

bool func::HasKey() { return ks.valid; }
bool func::LastOk() { return ok; }
String^ func::LastLog() { return W(log); }

String^ func::DescribeKeys() {
	if (!ks.valid)
		return L"Ключ користувача не задано.";
	return W(crypto::describeKeys(ks));
}

String^ func::DescribeRsaKeys() {
	return W(crypto::describeRsa(own) + L"\n" + crypto::describeRsa(other));
}

String^ func::SelfTest() {
	return W(crypto::selfTest(own, other));
}

// ---------- файли ----------

String^ func::EncryptFile(String^ inPath, String^ outPath) {
	crypto::FileReport r = crypto::processFile(ToW(inPath), ToW(outPath), ks, true);
	ok = r.ok;
	log = r.message + L"\n" + r.trace;
	return W(log);
}

String^ func::DecryptFile(String^ inPath, String^ outPath) {
	crypto::FileReport r = crypto::processFile(ToW(inPath), ToW(outPath), ks, false);
	ok = r.ok;
	log = r.message + L"\n" + r.trace;
	return W(log);
}

// шифруємо файл у тимчасовий, розшифровуємо назад і порівнюємо з оригіналом
String^ func::VerifyFile(String^ inPath) {
	ok = false;
	log.clear();

	if (!ks.valid) {
		log = L"Помилка: не задано ключ користувача.";
		return W(log);
	}

	std::wstring src = ToW(inPath);
	std::wstring enc = src + L".verify.enc";
	std::wstring dec = src + L".verify.dec";

	crypto::FileReport r1 = crypto::processFile(src, enc, ks, true);
	if (!r1.ok) { log = r1.message; return W(log); }

	crypto::FileReport r2 = crypto::processFile(enc, dec, ks, false);
	if (!r2.ok) { log = r2.message; return W(log); }

	// порівнюємо побайтово
	std::ifstream f1(src.c_str(), std::ios::binary);
	std::ifstream f2(dec.c_str(), std::ios::binary);
	bool same = (f1.good() && f2.good());
	unsigned long long compared = 0;
	while (same) {
		char a[4096], b[4096];
		f1.read(a, sizeof(a));
		f2.read(b, sizeof(b));
		std::streamsize na = f1.gcount(), nb = f2.gcount();
		if (na != nb) { same = false; break; }
		if (na == 0) break;
		for (std::streamsize i = 0; i < na; ++i)
			if (a[i] != b[i]) { same = false; break; }
		compared += static_cast<unsigned long long>(na);
	}
	f1.close();
	f2.close();

	// прибираємо тимчасові файли
	_wremove(enc.c_str());
	_wremove(dec.c_str());

	std::wostringstream os;
	os << L"КОНТРОЛЬНИЙ ПРОГІН НАД ФАЙЛОМ\n";
	os << L"  Файл ................ " << src << L"\n";
	os << L"  Розмір .............. " << r1.bytes << L" байт(ів)\n";
	os << L"  Зашифровано ......... " << r1.bytes << L" байт(ів)\n";
	os << L"  Розшифровано ........ " << r2.bytes << L" байт(ів)\n";
	os << L"  Звірено з оригіналом  " << compared << L" байт(ів)\n";
	os << L"  РЕЗУЛЬТАТ ........... "
		<< (same ? L"збігається, перетворення оборотне"
			: L"РОЗБІЖНІСТЬ!") << L"\n";
	os << L"\nПротокол шифрування:" << r1.trace;
	os << L"\nПротокол розшифрування:" << r2.trace;

	ok = same;
	log = os.str();
	return W(log);
}

// ---------- текст ----------
// після шифрування байти бувають недруковані, тому показуємо їх у hex

String^ func::EncryptTextToHex(String^ text) {
	ok = false;
	log.clear();

	if (!ks.valid) { log = L"Помилка: не задано ключ користувача."; return L""; }

	std::string src = ToBytes(text);
	if (src.empty()) { log = L"Помилка: порожній текст."; return L""; }

	std::wstring trace;
	trace += L"Вхідний текст у байтах UTF-8 (" + std::to_wstring(src.size()) + L" Б):\n  "
		+ crypto::bytesToHex(src) + L"\n";

	std::string enc = crypto::processBytes(src, ks, true, &trace);

	ok = true;
	log = trace;
	return W(crypto::bytesToHex(enc));
}

String^ func::DecryptHexToText(String^ hex) {
	ok = false;
	log.clear();

	if (!ks.valid) { log = L"Помилка: не задано ключ користувача."; return L""; }

	bool parsed = false;
	std::string src = crypto::hexToBytes(ToW(hex), &parsed);
	if (!parsed) {
		log = L"Помилка: шифртекст має бути послідовністю шістнадцяткових цифр "
			L"(парна кількість, пробіли дозволені).";
		return L"";
	}
	if (src.empty()) { log = L"Помилка: порожній шифртекст."; return L""; }

	std::wstring trace;
	std::string dec = crypto::processBytes(src, ks, false, &trace);
	trace += L"\nВідновлені байти UTF-8 (" + std::to_wstring(dec.size()) + L" Б):\n  "
		+ crypto::bytesToHex(dec) + L"\n";

	ok = true;
	log = trace;
	return FromBytes(dec);
}

// ---------- RSA ----------

String^ func::RsaProtect(String^ message) {
	log.clear();
	bool good = false;
	std::wstring trace;
	std::wstring result = crypto::rsaProtect(ToBytes(message), own, other, &trace, &good);
	ok = good;
	log = trace;
	return W(result);
}

String^ func::RsaUnprotect(String^ cipher) {
	log.clear();
	bool good = false;
	std::wstring trace;
	std::string result = crypto::rsaUnprotect(ToW(cipher), own, other, &trace, &good);
	ok = good;
	log = trace;
	return good ? FromBytes(result) : L"";
}
