// MainForm.h - вікно програми: вкладки "Файл", "Текст", "RSA", "Про програму"
// Юхно О.В., 401-ТН
#pragma once
#include <string>
#include "func.h"

namespace clrmath {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	public ref class MainForm : public System::Windows::Forms::Form
	{
	public:
		MainForm(void)
		{
			InitializeComponent();
			// func - звичайний C++ клас, тому тримаємо через вказівник
			engine = new func();
			ShowAbout();
			this->txtFileLog->Text = engine->SelfTest()->Replace(L"\n", L"\r\n")
				+ L"\r\n\r\nВведіть ключ користувача та оберіть файл.";
		}

	protected:
		~MainForm()
		{
			if (components) delete components;
			if (engine) { delete engine; engine = nullptr; }
		}

	private:
		func* engine;

		// верхня панель: ключ
		System::Windows::Forms::Label^    lblKey;
		System::Windows::Forms::TextBox^  txtKey;
		System::Windows::Forms::Button^   btnKeys;

		System::Windows::Forms::TabControl^ tabs;
		System::Windows::Forms::TabPage^    tabFile;
		System::Windows::Forms::TabPage^    tabText;
		System::Windows::Forms::TabPage^    tabRsa;
		System::Windows::Forms::TabPage^    tabAbout;

		// вкладка "Файл"
		System::Windows::Forms::Label^   lblIn;
		System::Windows::Forms::TextBox^ txtInFile;
		System::Windows::Forms::Button^  btnBrowseIn;
		System::Windows::Forms::Label^   lblOut;
		System::Windows::Forms::TextBox^ txtOutFile;
		System::Windows::Forms::Button^  btnBrowseOut;
		System::Windows::Forms::Button^  btnFileEnc;
		System::Windows::Forms::Button^  btnFileDec;
		System::Windows::Forms::Button^  btnFileVerify;
		System::Windows::Forms::TextBox^ txtFileLog;

		// вкладка "Текст"
		System::Windows::Forms::Label^   lblPlain;
		System::Windows::Forms::TextBox^ txtPlain;
		System::Windows::Forms::Button^  btnTextEnc;
		System::Windows::Forms::Button^  btnTextDec;
		System::Windows::Forms::Label^   lblCipher;
		System::Windows::Forms::TextBox^ txtCipher;
		System::Windows::Forms::TextBox^ txtTextLog;

		// вкладка "RSA"
		System::Windows::Forms::Label^   lblRsaIn;
		System::Windows::Forms::TextBox^ txtRsaIn;
		System::Windows::Forms::Button^  btnRsaProtect;
		System::Windows::Forms::Button^  btnRsaUnprotect;
		System::Windows::Forms::Button^  btnRsaKeys;
		System::Windows::Forms::Label^   lblRsaOut;
		System::Windows::Forms::TextBox^ txtRsaOut;
		System::Windows::Forms::TextBox^ txtRsaLog;

		// вкладка "Про програму"
		System::Windows::Forms::TextBox^ txtAbout;

		System::ComponentModel::Container^ components;

#pragma region Windows Form Designer generated code
		void InitializeComponent(void)
		{
			System::ComponentModel::ComponentResourceManager^ resources =
				(gcnew System::ComponentModel::ComponentResourceManager(MainForm::typeid));

			System::Drawing::Font^ uiFont = gcnew System::Drawing::Font(
				L"Segoe UI", 10.0F, System::Drawing::FontStyle::Regular,
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(204));
			System::Drawing::Font^ uiBold = gcnew System::Drawing::Font(
				L"Segoe UI", 10.0F, System::Drawing::FontStyle::Bold,
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(204));
			System::Drawing::Font^ monoFont = gcnew System::Drawing::Font(
				L"Consolas", 9.5F, System::Drawing::FontStyle::Regular,
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(204));

			this->lblKey = (gcnew System::Windows::Forms::Label());
			this->txtKey = (gcnew System::Windows::Forms::TextBox());
			this->btnKeys = (gcnew System::Windows::Forms::Button());
			this->tabs = (gcnew System::Windows::Forms::TabControl());
			this->tabFile = (gcnew System::Windows::Forms::TabPage());
			this->tabText = (gcnew System::Windows::Forms::TabPage());
			this->tabRsa = (gcnew System::Windows::Forms::TabPage());
			this->tabAbout = (gcnew System::Windows::Forms::TabPage());
			this->lblIn = (gcnew System::Windows::Forms::Label());
			this->txtInFile = (gcnew System::Windows::Forms::TextBox());
			this->btnBrowseIn = (gcnew System::Windows::Forms::Button());
			this->lblOut = (gcnew System::Windows::Forms::Label());
			this->txtOutFile = (gcnew System::Windows::Forms::TextBox());
			this->btnBrowseOut = (gcnew System::Windows::Forms::Button());
			this->btnFileEnc = (gcnew System::Windows::Forms::Button());
			this->btnFileDec = (gcnew System::Windows::Forms::Button());
			this->btnFileVerify = (gcnew System::Windows::Forms::Button());
			this->txtFileLog = (gcnew System::Windows::Forms::TextBox());
			this->lblPlain = (gcnew System::Windows::Forms::Label());
			this->txtPlain = (gcnew System::Windows::Forms::TextBox());
			this->btnTextEnc = (gcnew System::Windows::Forms::Button());
			this->btnTextDec = (gcnew System::Windows::Forms::Button());
			this->lblCipher = (gcnew System::Windows::Forms::Label());
			this->txtCipher = (gcnew System::Windows::Forms::TextBox());
			this->txtTextLog = (gcnew System::Windows::Forms::TextBox());
			this->lblRsaIn = (gcnew System::Windows::Forms::Label());
			this->txtRsaIn = (gcnew System::Windows::Forms::TextBox());
			this->btnRsaProtect = (gcnew System::Windows::Forms::Button());
			this->btnRsaUnprotect = (gcnew System::Windows::Forms::Button());
			this->btnRsaKeys = (gcnew System::Windows::Forms::Button());
			this->lblRsaOut = (gcnew System::Windows::Forms::Label());
			this->txtRsaOut = (gcnew System::Windows::Forms::TextBox());
			this->txtRsaLog = (gcnew System::Windows::Forms::TextBox());
			this->txtAbout = (gcnew System::Windows::Forms::TextBox());

			this->tabs->SuspendLayout();
			this->tabFile->SuspendLayout();
			this->tabText->SuspendLayout();
			this->tabRsa->SuspendLayout();
			this->tabAbout->SuspendLayout();
			this->SuspendLayout();

			// верхня панель: ключ користувача
			this->lblKey->AutoSize = true;
			this->lblKey->Font = uiBold;
			this->lblKey->Location = System::Drawing::Point(16, 14);
			this->lblKey->Text = L"Ключ користувача (один довільний ключ, з нього утворюються всі підключі):";

			this->txtKey->Font = monoFont;
			this->txtKey->Location = System::Drawing::Point(16, 40);
			this->txtKey->Size = System::Drawing::Size(700, 25);
			this->txtKey->Text = L"";

			this->btnKeys->Font = uiFont;
			this->btnKeys->Location = System::Drawing::Point(728, 38);
			this->btnKeys->Size = System::Drawing::Size(256, 30);
			this->btnKeys->Text = L"Показати похідні ключі";
			this->btnKeys->UseVisualStyleBackColor = true;
			this->btnKeys->Click += gcnew System::EventHandler(this, &MainForm::btnKeys_Click);

			// вкладка "Файл"
			this->lblIn->AutoSize = true;
			this->lblIn->Font = uiFont;
			this->lblIn->Location = System::Drawing::Point(14, 18);
			this->lblIn->Text = L"Вхідний файл:";

			this->txtInFile->Font = monoFont;
			this->txtInFile->Location = System::Drawing::Point(140, 14);
			this->txtInFile->Size = System::Drawing::Size(676, 25);

			this->btnBrowseIn->Font = uiFont;
			this->btnBrowseIn->Location = System::Drawing::Point(824, 12);
			this->btnBrowseIn->Size = System::Drawing::Size(110, 28);
			this->btnBrowseIn->Text = L"Огляд...";
			this->btnBrowseIn->UseVisualStyleBackColor = true;
			this->btnBrowseIn->Click += gcnew System::EventHandler(this, &MainForm::btnBrowseIn_Click);

			this->lblOut->AutoSize = true;
			this->lblOut->Font = uiFont;
			this->lblOut->Location = System::Drawing::Point(14, 54);
			this->lblOut->Text = L"Вихідний файл:";

			this->txtOutFile->Font = monoFont;
			this->txtOutFile->Location = System::Drawing::Point(140, 50);
			this->txtOutFile->Size = System::Drawing::Size(676, 25);

			this->btnBrowseOut->Font = uiFont;
			this->btnBrowseOut->Location = System::Drawing::Point(824, 48);
			this->btnBrowseOut->Size = System::Drawing::Size(110, 28);
			this->btnBrowseOut->Text = L"Огляд...";
			this->btnBrowseOut->UseVisualStyleBackColor = true;
			this->btnBrowseOut->Click += gcnew System::EventHandler(this, &MainForm::btnBrowseOut_Click);

			this->btnFileEnc->Font = uiBold;
			this->btnFileEnc->Location = System::Drawing::Point(14, 88);
			this->btnFileEnc->Size = System::Drawing::Size(300, 42);
			this->btnFileEnc->Text = L"Зашифрувати файл";
			this->btnFileEnc->UseVisualStyleBackColor = true;
			this->btnFileEnc->Click += gcnew System::EventHandler(this, &MainForm::btnFileEnc_Click);

			this->btnFileDec->Font = uiBold;
			this->btnFileDec->Location = System::Drawing::Point(324, 88);
			this->btnFileDec->Size = System::Drawing::Size(300, 42);
			this->btnFileDec->Text = L"Розшифрувати файл";
			this->btnFileDec->UseVisualStyleBackColor = true;
			this->btnFileDec->Click += gcnew System::EventHandler(this, &MainForm::btnFileDec_Click);

			this->btnFileVerify->Font = uiFont;
			this->btnFileVerify->Location = System::Drawing::Point(634, 88);
			this->btnFileVerify->Size = System::Drawing::Size(300, 42);
			this->btnFileVerify->Text = L"Контрольний прогін (шифр → дешифр)";
			this->btnFileVerify->UseVisualStyleBackColor = true;
			this->btnFileVerify->Click += gcnew System::EventHandler(this, &MainForm::btnFileVerify_Click);

			this->txtFileLog->Font = monoFont;
			this->txtFileLog->Location = System::Drawing::Point(14, 142);
			this->txtFileLog->Size = System::Drawing::Size(920, 456);
			this->txtFileLog->Multiline = true;
			this->txtFileLog->ReadOnly = true;
			this->txtFileLog->WordWrap = false;
			this->txtFileLog->ScrollBars = System::Windows::Forms::ScrollBars::Both;
			this->txtFileLog->BackColor = System::Drawing::Color::White;

			this->tabFile->Text = L"Файл (комбінований шифр)";
			this->tabFile->Font = uiFont;
			this->tabFile->UseVisualStyleBackColor = true;
			this->tabFile->Controls->Add(this->lblIn);
			this->tabFile->Controls->Add(this->txtInFile);
			this->tabFile->Controls->Add(this->btnBrowseIn);
			this->tabFile->Controls->Add(this->lblOut);
			this->tabFile->Controls->Add(this->txtOutFile);
			this->tabFile->Controls->Add(this->btnBrowseOut);
			this->tabFile->Controls->Add(this->btnFileEnc);
			this->tabFile->Controls->Add(this->btnFileDec);
			this->tabFile->Controls->Add(this->btnFileVerify);
			this->tabFile->Controls->Add(this->txtFileLog);

			// вкладка "Текст"
			this->lblPlain->AutoSize = true;
			this->lblPlain->Font = uiFont;
			this->lblPlain->Location = System::Drawing::Point(14, 14);
			this->lblPlain->Text = L"Відкритий текст:";

			this->txtPlain->Font = uiFont;
			this->txtPlain->Location = System::Drawing::Point(14, 40);
			this->txtPlain->Size = System::Drawing::Size(920, 130);
			this->txtPlain->Multiline = true;
			this->txtPlain->ScrollBars = System::Windows::Forms::ScrollBars::Vertical;

			this->btnTextEnc->Font = uiBold;
			this->btnTextEnc->Location = System::Drawing::Point(14, 180);
			this->btnTextEnc->Size = System::Drawing::Size(452, 38);
			this->btnTextEnc->Text = L"Зашифрувати  ↓";
			this->btnTextEnc->UseVisualStyleBackColor = true;
			this->btnTextEnc->Click += gcnew System::EventHandler(this, &MainForm::btnTextEnc_Click);

			this->btnTextDec->Font = uiBold;
			this->btnTextDec->Location = System::Drawing::Point(482, 180);
			this->btnTextDec->Size = System::Drawing::Size(452, 38);
			this->btnTextDec->Text = L"↑  Розшифрувати";
			this->btnTextDec->UseVisualStyleBackColor = true;
			this->btnTextDec->Click += gcnew System::EventHandler(this, &MainForm::btnTextDec_Click);

			this->lblCipher->AutoSize = true;
			this->lblCipher->Font = uiFont;
			this->lblCipher->Location = System::Drawing::Point(14, 228);
			this->lblCipher->Text = L"Шифртекст (шістнадцяткові коди байтів):";

			this->txtCipher->Font = monoFont;
			this->txtCipher->Location = System::Drawing::Point(14, 254);
			this->txtCipher->Size = System::Drawing::Size(920, 110);
			this->txtCipher->Multiline = true;
			this->txtCipher->ScrollBars = System::Windows::Forms::ScrollBars::Vertical;

			this->txtTextLog->Font = monoFont;
			this->txtTextLog->Location = System::Drawing::Point(14, 376);
			this->txtTextLog->Size = System::Drawing::Size(920, 222);
			this->txtTextLog->Multiline = true;
			this->txtTextLog->ReadOnly = true;
			this->txtTextLog->WordWrap = false;
			this->txtTextLog->ScrollBars = System::Windows::Forms::ScrollBars::Both;
			this->txtTextLog->BackColor = System::Drawing::Color::White;

			this->tabText->Text = L"Текст (комбінований шифр)";
			this->tabText->Font = uiFont;
			this->tabText->UseVisualStyleBackColor = true;
			this->tabText->Controls->Add(this->lblPlain);
			this->tabText->Controls->Add(this->txtPlain);
			this->tabText->Controls->Add(this->btnTextEnc);
			this->tabText->Controls->Add(this->btnTextDec);
			this->tabText->Controls->Add(this->lblCipher);
			this->tabText->Controls->Add(this->txtCipher);
			this->tabText->Controls->Add(this->txtTextLog);

			// вкладка "RSA"
			this->lblRsaIn->AutoSize = true;
			this->lblRsaIn->Font = uiFont;
			this->lblRsaIn->Location = System::Drawing::Point(14, 14);
			this->lblRsaIn->Text = L"Повідомлення:";

			this->txtRsaIn->Font = uiFont;
			this->txtRsaIn->Location = System::Drawing::Point(14, 40);
			this->txtRsaIn->Size = System::Drawing::Size(920, 100);
			this->txtRsaIn->Multiline = true;
			this->txtRsaIn->ScrollBars = System::Windows::Forms::ScrollBars::Vertical;

			this->btnRsaProtect->Font = uiBold;
			this->btnRsaProtect->Location = System::Drawing::Point(14, 150);
			this->btnRsaProtect->Size = System::Drawing::Size(452, 44);
			this->btnRsaProtect->Text = L"Свій ЗАКРИТИЙ → чужий ВІДКРИТИЙ  ↓";
			this->btnRsaProtect->UseVisualStyleBackColor = true;
			this->btnRsaProtect->Click += gcnew System::EventHandler(this, &MainForm::btnRsaProtect_Click);

			this->btnRsaUnprotect->Font = uiBold;
			this->btnRsaUnprotect->Location = System::Drawing::Point(482, 150);
			this->btnRsaUnprotect->Size = System::Drawing::Size(452, 44);
			this->btnRsaUnprotect->Text = L"↑  Чужий ЗАКРИТИЙ → свій ВІДКРИТИЙ";
			this->btnRsaUnprotect->UseVisualStyleBackColor = true;
			this->btnRsaUnprotect->Click += gcnew System::EventHandler(this, &MainForm::btnRsaUnprotect_Click);

			this->lblRsaOut->AutoSize = true;
			this->lblRsaOut->Font = uiFont;
			this->lblRsaOut->Location = System::Drawing::Point(14, 204);
			this->lblRsaOut->Text = L"Шифртекст (числа, розділені пробілами):";

			this->txtRsaOut->Font = monoFont;
			this->txtRsaOut->Location = System::Drawing::Point(14, 230);
			this->txtRsaOut->Size = System::Drawing::Size(920, 100);
			this->txtRsaOut->Multiline = true;
			this->txtRsaOut->ScrollBars = System::Windows::Forms::ScrollBars::Vertical;

			this->btnRsaKeys->Font = uiFont;
			this->btnRsaKeys->Location = System::Drawing::Point(14, 340);
			this->btnRsaKeys->Size = System::Drawing::Size(300, 30);
			this->btnRsaKeys->Text = L"Показати обидві пари ключів";
			this->btnRsaKeys->UseVisualStyleBackColor = true;
			this->btnRsaKeys->Click += gcnew System::EventHandler(this, &MainForm::btnRsaKeys_Click);

			this->txtRsaLog->Font = monoFont;
			this->txtRsaLog->Location = System::Drawing::Point(14, 378);
			this->txtRsaLog->Size = System::Drawing::Size(920, 220);
			this->txtRsaLog->Multiline = true;
			this->txtRsaLog->ReadOnly = true;
			this->txtRsaLog->WordWrap = false;
			this->txtRsaLog->ScrollBars = System::Windows::Forms::ScrollBars::Both;
			this->txtRsaLog->BackColor = System::Drawing::Color::White;

			this->tabRsa->Text = L"RSA (дві пари ключів)";
			this->tabRsa->Font = uiFont;
			this->tabRsa->UseVisualStyleBackColor = true;
			this->tabRsa->Controls->Add(this->lblRsaIn);
			this->tabRsa->Controls->Add(this->txtRsaIn);
			this->tabRsa->Controls->Add(this->btnRsaProtect);
			this->tabRsa->Controls->Add(this->btnRsaUnprotect);
			this->tabRsa->Controls->Add(this->lblRsaOut);
			this->tabRsa->Controls->Add(this->txtRsaOut);
			this->tabRsa->Controls->Add(this->btnRsaKeys);
			this->tabRsa->Controls->Add(this->txtRsaLog);

			// вкладка "Про програму"
			this->txtAbout->Font = monoFont;
			this->txtAbout->Location = System::Drawing::Point(14, 14);
			this->txtAbout->Size = System::Drawing::Size(920, 584);
			this->txtAbout->Multiline = true;
			this->txtAbout->ReadOnly = true;
			this->txtAbout->WordWrap = false;
			this->txtAbout->ScrollBars = System::Windows::Forms::ScrollBars::Both;
			this->txtAbout->BackColor = System::Drawing::Color::White;

			this->tabAbout->Text = L"Про програму";
			this->tabAbout->Font = uiFont;
			this->tabAbout->UseVisualStyleBackColor = true;
			this->tabAbout->Controls->Add(this->txtAbout);

			// панель вкладок
			this->tabs->Font = uiFont;
			this->tabs->Location = System::Drawing::Point(16, 80);
			this->tabs->Size = System::Drawing::Size(968, 640);
			this->tabs->SelectedIndex = 0;
			this->tabs->Controls->Add(this->tabFile);
			this->tabs->Controls->Add(this->tabText);
			this->tabs->Controls->Add(this->tabRsa);
			this->tabs->Controls->Add(this->tabAbout);

			// вікно
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::None;
			this->ClientSize = System::Drawing::Size(1000, 736);
			this->Controls->Add(this->lblKey);
			this->Controls->Add(this->txtKey);
			this->Controls->Add(this->btnKeys);
			this->Controls->Add(this->tabs);
			this->Icon = (cli::safe_cast<System::Drawing::Icon^>(resources->GetObject(L"$this.Icon")));
			this->Name = L"MainForm";
			this->Text = L"РГР з ТЗІ - комбіноване шифрування та RSA (Юхно О.В., гр. 401-ТН)";
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedSingle;
			this->MaximizeBox = false;
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;

			this->tabFile->ResumeLayout(false);
			this->tabFile->PerformLayout();
			this->tabText->ResumeLayout(false);
			this->tabText->PerformLayout();
			this->tabRsa->ResumeLayout(false);
			this->tabRsa->PerformLayout();
			this->tabAbout->ResumeLayout(false);
			this->tabAbout->PerformLayout();
			this->tabs->ResumeLayout(false);
			this->ResumeLayout(false);
			this->PerformLayout();
		}
#pragma endregion

		// ---------- допоміжні методи ----------

		// бере ключ з поля txtKey; якщо він порожній, показує попередження
		bool ApplyKey()
		{
			engine->SetUserKey(this->txtKey->Text);
			if (!engine->HasKey()) {
				MessageBox::Show(this,
					L"Спочатку введіть ключ користувача.\n\n"
					L"З цього одного ключа програма сама утворює:\n"
					L"  - коефіцієнти a та b афінного перетворення;\n"
					L"  - таблицю перестановки блоку.",
					L"Не задано ключ",
					MessageBoxButtons::OK, MessageBoxIcon::Warning);
				return false;
			}
			return true;
		}

		void ShowAbout()
		{
			String^ nl = L"\r\n";
			this->txtAbout->Text =
				L"===============================================================" + nl +
				L"  РОЗРАХУНКОВО-ГРАФІЧНА РОБОТА З ДИСЦИПЛІНИ" + nl +
				L"  \"ТЕХНОЛОГІЇ ЗАХИСТУ ІНФОРМАЦІЇ\"" + nl +
				L"  Тема: організація захисту інформації." + nl +
				L"        Дослідження методів шифрування." + nl +
				L"===============================================================" + nl + nl +
				L"  Автор:     Юхно О.В., студент групи 401-ТН" + nl +
				L"  Заклад:    Національний університет" + nl +
				L"             \"Полтавська політехніка імені Юрія Кондратюка\"" + nl +
				L"  Кафедра:   комп'ютерних та інформаційних технологій і систем" + nl + nl +
				L"---------------------------------------------------------------" + nl +
				L"  ХАРАКТЕРИСТИКА ПРОГРАМИ" + nl +
				L"---------------------------------------------------------------" + nl + nl +
				L"  Програма реалізує дві незалежні частини завдання." + nl + nl +
				L"  ЧАСТИНА 1. Комбіноване шифрування файлу довільної структури" + nl +
				L"  (.exe, .txt, зображення тощо) двома різними методами." + nl + nl +
				L"    Метод 1 - шифрування аналітичними перетвореннями:" + nl +
				L"              афінний шифр  C = (a*P + b) mod 256." + nl +
				L"              Перетворення оборотне, бо НСД(a, 256) = 1." + nl + nl +
				L"    Метод 2 - шифрування методом простої перестановки:" + nl +
				L"              байти блоку з 16 байтів переставляються за" + nl +
				L"              таблицею, похідною від ключа користувача." + nl + nl +
				L"    Порядок обробки відповідає завданню: для кожного блоку" + nl +
				L"    послідовно застосовується спочатку перший метод, потім" + nl +
				L"    другий, і лише після цього блок записується у вихідний" + nl +
				L"    файл. Файл обробляється потоково, тому його розмір" + nl +
				L"    не обмежений обсягом пам'яті." + nl + nl +
				L"    Усі підключі (a, b і таблиця перестановки) утворюються" + nl +
				L"    з ОДНОГО довільного ключа, який задає користувач:" + nl +
				L"        рядок ключа -> хеш FNV-1a -> зерно -> xorshift32 ->" + nl +
				L"        -> a, b, тасування Фішера-Йєтса." + nl + nl +
				L"  ЧАСТИНА 2. Криптосистема RSA з двома парами ключів." + nl + nl +
				L"    Захист повідомлення (схема \"підписати, потім зашифрувати\"):" + nl +
				L"        S = M^d_свій   mod n_свій     - своїм ЗАКРИТИМ" + nl +
				L"        C = S^e_чужий  mod n_чужий    - чужим ВІДКРИТИМ" + nl + nl +
				L"    Зняття захисту виконується у зворотному порядку:" + nl +
				L"        S = C^d_чужий  mod n_чужий    - чужим ЗАКРИТИМ" + nl +
				L"        M = S^e_свій   mod n_свій     - своїм ВІДКРИТИМ" + nl + nl +
				L"    Умова оборотності ланцюжка: n_свій < n_чужий." + nl + nl +
				L"---------------------------------------------------------------" + nl +
				L"  СТРУКТУРА ВИХІДНОГО КОДУ" + nl +
				L"---------------------------------------------------------------" + nl + nl +
				L"    crypto_core.h - обчислювальне ядро (чистий C++):" + nl +
				L"                    теорія чисел, генератор ключів, обидва" + nl +
				L"                    методи шифрування, RSA, самоперевірка;" + nl +
				L"    func.h /" + nl +
				L"    Source.cpp    - прошарок між ядром та інтерфейсом;" + nl +
				L"    MainForm.h    - вікно програми;" + nl +
				L"    MainForm.cpp  - точка входу." + nl + nl +
				L"---------------------------------------------------------------" + nl +
				L"  ЗАСТЕРЕЖЕННЯ ЩОДО СТІЙКОСТІ" + nl +
				L"---------------------------------------------------------------" + nl + nl +
				L"  Параметри RSA (p = 61, q = 53 та p = 71, q = 47) обрано" + nl +
				L"  навмисно малими, щоб у звіті можна було навести всі" + nl +
				L"  проміжні обчислення. Модуль n такого розміру розкладається" + nl +
				L"  на множники усно, тож навчальні ключі криптостійкості" + nl +
				L"  не мають. Це пряма ілюстрація атаки факторизації, яка" + nl +
				L"  розглядається в теоретичній частині роботи." + nl + nl +
				L"  Комбінований шифр також є навчальним: афінне перетворення" + nl +
				L"  та перестановка фіксованого блоку не приховують статистику" + nl +
				L"  відкритого тексту настільки, як сучасні блокові шифри." + nl;
			this->txtAbout->Select(0, 0);
		}

		// ---------- обробники подій ----------

		System::Void btnKeys_Click(System::Object^ sender, System::EventArgs^ e)
		{
			if (!ApplyKey()) return;
			this->txtFileLog->Text = engine->DescribeKeys();
			this->txtTextLog->Text = engine->DescribeKeys();
			this->tabs->SelectedTab = this->tabFile;
		}

		System::Void btnBrowseIn_Click(System::Object^ sender, System::EventArgs^ e)
		{
			OpenFileDialog^ dlg = gcnew OpenFileDialog();
			dlg->Title = L"Оберіть вхідний файл";
			dlg->Filter = L"Усі файли (*.*)|*.*";
			if (dlg->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
				this->txtInFile->Text = dlg->FileName;
				// підказуємо ім'я вихідного файлу
				if (String::IsNullOrEmpty(this->txtOutFile->Text)) {
					if (dlg->FileName->EndsWith(L".enc"))
						this->txtOutFile->Text =
						dlg->FileName->Substring(0, dlg->FileName->Length - 4) + L".dec";
					else
						this->txtOutFile->Text = dlg->FileName + L".enc";
				}
			}
		}

		System::Void btnBrowseOut_Click(System::Object^ sender, System::EventArgs^ e)
		{
			SaveFileDialog^ dlg = gcnew SaveFileDialog();
			dlg->Title = L"Оберіть вихідний файл";
			dlg->Filter = L"Усі файли (*.*)|*.*";
			dlg->OverwritePrompt = true;
			if (dlg->ShowDialog() == System::Windows::Forms::DialogResult::OK)
				this->txtOutFile->Text = dlg->FileName;
		}

		System::Void btnFileEnc_Click(System::Object^ sender, System::EventArgs^ e)
		{
			if (!ApplyKey()) return;
			this->txtFileLog->Text = engine->DescribeKeys() + L"\r\n"
				+ engine->EncryptFile(this->txtInFile->Text, this->txtOutFile->Text);
			this->txtFileLog->Select(0, 0);
			this->txtFileLog->ScrollToCaret();
		}

		System::Void btnFileDec_Click(System::Object^ sender, System::EventArgs^ e)
		{
			if (!ApplyKey()) return;
			this->txtFileLog->Text = engine->DescribeKeys() + L"\r\n"
				+ engine->DecryptFile(this->txtInFile->Text, this->txtOutFile->Text);
			this->txtFileLog->Select(0, 0);
			this->txtFileLog->ScrollToCaret();
		}

		System::Void btnFileVerify_Click(System::Object^ sender, System::EventArgs^ e)
		{
			if (!ApplyKey()) return;
			this->txtFileLog->Text = engine->DescribeKeys() + L"\r\n"
				+ engine->VerifyFile(this->txtInFile->Text);
			this->txtFileLog->Select(0, 0);
			this->txtFileLog->ScrollToCaret();
		}

		System::Void btnTextEnc_Click(System::Object^ sender, System::EventArgs^ e)
		{
			if (!ApplyKey()) return;
			String^ hex = engine->EncryptTextToHex(this->txtPlain->Text);
			this->txtCipher->Text = hex;
			this->txtTextLog->Text = engine->DescribeKeys() + L"\r\n" + engine->LastLog();
			this->txtTextLog->Select(0, 0);
			this->txtTextLog->ScrollToCaret();
		}

		System::Void btnTextDec_Click(System::Object^ sender, System::EventArgs^ e)
		{
			if (!ApplyKey()) return;
			String^ text = engine->DecryptHexToText(this->txtCipher->Text);
			if (engine->LastOk()) this->txtPlain->Text = text;
			this->txtTextLog->Text = engine->DescribeKeys() + L"\r\n" + engine->LastLog();
			this->txtTextLog->Select(0, 0);
			this->txtTextLog->ScrollToCaret();
		}

		System::Void btnRsaKeys_Click(System::Object^ sender, System::EventArgs^ e)
		{
			this->txtRsaLog->Text = engine->DescribeRsaKeys();
		}

		System::Void btnRsaProtect_Click(System::Object^ sender, System::EventArgs^ e)
		{
			String^ cipher = engine->RsaProtect(this->txtRsaIn->Text);
			if (engine->LastOk()) this->txtRsaOut->Text = cipher;
			this->txtRsaLog->Text = engine->DescribeRsaKeys() + L"\r\n" + engine->LastLog();
			this->txtRsaLog->Select(0, 0);
			this->txtRsaLog->ScrollToCaret();
		}

		System::Void btnRsaUnprotect_Click(System::Object^ sender, System::EventArgs^ e)
		{
			String^ text = engine->RsaUnprotect(this->txtRsaOut->Text);
			if (engine->LastOk()) this->txtRsaIn->Text = text;
			this->txtRsaLog->Text = engine->DescribeRsaKeys() + L"\r\n" + engine->LastLog();
			this->txtRsaLog->Select(0, 0);
			this->txtRsaLog->ScrollToCaret();
		}
	};
}
