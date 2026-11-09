// func.h - зв'язка між crypto_core.h і вікном програми (Windows Forms)
// Юхно О.В., 401-ТН
#ifndef HEADER_H
#define HEADER_H

#include <string>
#include <msclr/marshal_cppstd.h>
#include "crypto_core.h"

using namespace System;
using namespace System::Windows::Forms;

class func {
public:
	func();
	~func();

	// ключі
	void     SetUserKey(String^ key);   // ключ -> підключі
	bool     HasKey();
	String^  DescribeKeys();            // опис підключів
	String^  DescribeRsaKeys();         // опис обох пар ключів RSA
	String^  SelfTest();                // самоперевірка

	// комбінований шифр, файли
	String^  EncryptFile(String^ inPath, String^ outPath);
	String^  DecryptFile(String^ inPath, String^ outPath);
	String^  VerifyFile(String^ inPath);  // зашифрувати -> розшифрувати -> звірити

	// комбінований шифр, текст
	String^  EncryptTextToHex(String^ text);
	String^  DecryptHexToText(String^ hex);

	// RSA
	String^  RsaProtect(String^ message);    // свій закритий -> чужий відкритий
	String^  RsaUnprotect(String^ cipher);   // чужий закритий -> свій відкритий

	// результат останньої операції
	bool     LastOk();
	String^  LastLog();

	// лишилось зі старої версії
	std::string SystemStringToStdString(String^ sysString);
	String^     StdStringToSystemString(const std::string& stdString);

private:
	crypto::KeySchedule ks;      // підключі
	crypto::RsaKeyPair  own;     // своя пара RSA
	crypto::RsaKeyPair  other;   // чужа пара RSA
	std::wstring        log;     // журнал
	bool                ok;      // чи вдалася остання операція

	// String^ <-> байти UTF-8, щоб не залежати від кодової сторінки
	static std::string  ToBytes(String^ s);
	static String^      FromBytes(const std::string& s);
	// для TextBox потрібен \r\n замість \n
	static String^      W(const std::wstring& w);
	static std::wstring ToW(String^ s);
};

#endif
