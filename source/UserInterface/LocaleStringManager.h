#pragma once

class LocaleStringManager : public CSingleton<LocaleStringManager>
{
	#define STRING_SIZE 32 + 1

	public:
		enum Formats
		{
			FORMAT_TYPE_INT,
			FORMAT_TYPE_UNSIGNED_INT,
			FORMAT_TYPE_SHORT_INT,
			FORMAT_TYPE_SIGNED_CHAR,
			FORMAT_TYPE_LONG,
			FORMAT_TYPE_UNSIGNED_LONG,
			FORMAT_TYPE_LONG_INT,
			FORMAT_TYPE_LONG_LONG_INT,
			FORMAT_TYPE_FLOAT,
			FORMAT_TYPE_STRING,

			FORMAT_TYPE_MAX_NUM

		};

		enum ELocaleStrings
		{
			NUM_LOCALES = 2,
		};

		typedef struct SFormat
		{
			int				iType;
			char			szName[4 + 1];
			UINT			iSize;
		} TFormat;

		static TFormat	FormatTable[FORMAT_TYPE_MAX_NUM];
		bool			IsFormatSpecifier(const char* specifier);
		int				GetFormatSpecifierType(const char* specifier);
		UINT			GetFormatSpecifierSize(int type);
		int				GetFormatSpecifierStringSize(int type);
		void			LocateFormatSpecifier(const char* c_szFormat, size_t iSize, int& iSpecifierType, int& iEndPos);
		bool			FillLocaleString(DWORD dwStringIndex, std::string& strLocaleString);

		LocaleStringManager();
		virtual			~LocaleStringManager();

		std::string		FindLocaleString(DWORD dwStringIndex);
		bool			RegisterLocaleStringTable(const char * c_szFileName);

		bool			IsLoaded() { return m_bLoaded; };
		void			SetLoaded(bool isLoaded) { m_bLoaded = isLoaded; };

	private:
		typedef std::map<DWORD, std::string>	LocaleStringMapType;
		LocaleStringMapType	localeStringMap;
		LocaleStringMapType::iterator it;

		bool	m_bLoaded;
};