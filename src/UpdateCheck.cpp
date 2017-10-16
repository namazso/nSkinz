#include "UpdateCheck.hpp"
#include "Utilities/Platform.hpp"

#include <ctime>
#include <json.hpp>

using json = nlohmann::json;

namespace cx
{
	constexpr bool cx_strcmp_part(const char* const a, const char* const b)
	{
		return *b ? (*a == *b && cx_strcmp_part(a + 1, b + 1)) : true;
	}

	constexpr unsigned month_str_to_int(const char* const str)
	{
		return
			cx_strcmp_part(str, "Jan") ? 1 :
			cx_strcmp_part(str, "Feb") ? 2 :
			cx_strcmp_part(str, "Mar") ? 3 :
			cx_strcmp_part(str, "Apr") ? 4 :
			cx_strcmp_part(str, "May") ? 5 :
			cx_strcmp_part(str, "Jun") ? 6 :
			cx_strcmp_part(str, "Jul") ? 7 :
			cx_strcmp_part(str, "Aug") ? 8 :
			cx_strcmp_part(str, "Sep") ? 9 :
			cx_strcmp_part(str, "Oct") ? 10 :
			cx_strcmp_part(str, "Nov") ? 11 : 12;
	}

	constexpr unsigned char_to_int(const unsigned char c)
	{
		return (c == ' ' ? '0' : c) - '0';
	}

	constexpr unsigned twochar_to_int(const char* const str)
	{
		return (char_to_int(str[0])) * 10 + char_to_int(str[1]);
	}

	constexpr unsigned year_str_to_int(const char* const str)
	{
		return (char_to_int(str[0])) * 1000 + (char_to_int(str[1])) * 100 + (char_to_int(str[2])) * 10 + char_to_int(str[3]);
	}

	constexpr unsigned days_from_civil(int y, int m, int d)
	{
		const int yc = y - (m <= 2 ? 1 : 0);
		const int era = (yc >= 0 ? yc : yc - 399) / 400;
		const unsigned yoe = static_cast<unsigned>(yc - era * 400);				// [0, 399]
		const unsigned doy = (153 * (m + (m > 2 ? -3 : 9)) + 2) / 5 + d - 1;	// [0, 365]
		const unsigned doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;				// [0, 146096]
		return era * 146097 + int(doe) - 719468;
	}

	constexpr unsigned date_str_to_days(const char* const str)
	{
		return days_from_civil(year_str_to_int(str + 7), month_str_to_int(str), twochar_to_int(str + 4));
	}

	constexpr unsigned time_str_to_second(const char* const str)
	{
		return (twochar_to_int(str) * 60 + twochar_to_int(str + 3)) * 60 + twochar_to_int(str + 6);
	}

	constexpr uint64_t unix_time()
	{
		return uint64_t(date_str_to_days(__DATE__)) * 24 * 60 * 60 + time_str_to_second(__TIME__);
	}
}

using HTTPRequestHandle = uint32_t;
enum : HTTPRequestHandle { INVALID_HTTPREQUEST_HANDLE = 0 };

using SteamAPICall_t = uint64_t;
enum : SteamAPICall_t { k_uAPICallInvalid = 0 };

enum class EHTTPMethod
{
	Invalid = 0,
	GET,
	HEAD,
	POST,
	PUT,
	DELETE,
	OPTIONS,
	PATCH
};

class ISteamHTTP
{
public:
	virtual HTTPRequestHandle CreateHTTPRequest(EHTTPMethod eHTTPRequestMethod, const char* pchAbsoluteURL) = 0;
	virtual bool SetHTTPRequestContextValue(HTTPRequestHandle hRequest, uint64_t ulContextValue) = 0;
	virtual bool SetHTTPRequestNetworkActivityTimeout(HTTPRequestHandle hRequest, uint32_t unTimeoutSeconds) = 0;
	virtual bool SetHTTPRequestHeaderValue(HTTPRequestHandle hRequest, const char* pchHeaderName, const char* pchHeaderValue) = 0;
	virtual bool SetHTTPRequestGetOrPostParameter(HTTPRequestHandle hRequest, const char* pchParamName, const char* pchParamValue) = 0;
	virtual bool SendHTTPRequest(HTTPRequestHandle hRequest, SteamAPICall_t* pCallHandle) = 0;
	virtual bool SendHTTPRequestAndStreamResponse(HTTPRequestHandle hRequest, SteamAPICall_t* pCallHandle) = 0;
	virtual bool DeferHTTPRequest(HTTPRequestHandle hRequest) = 0;
	virtual bool PrioritizeHTTPRequest(HTTPRequestHandle hRequest) = 0;
	virtual bool GetHTTPResponseHeaderSize(HTTPRequestHandle hRequest, const char* pchHeaderName, uint32_t* unResponseHeaderSize) = 0;
	virtual bool GetHTTPResponseHeaderValue(HTTPRequestHandle hRequest, const char* pchHeaderName, uint8_t* pHeaderValueBuffer, uint32_t unBufferSize) = 0;
	virtual bool GetHTTPResponseBodySize(HTTPRequestHandle hRequest, uint32_t* unBodySize) = 0;
	virtual bool GetHTTPResponseBodyData(HTTPRequestHandle hRequest, uint8_t* pBodyDataBuffer, uint32_t unBufferSize) = 0;
	virtual bool GetHTTPStreamingResponseBodyData(HTTPRequestHandle hRequest, uint32_t cOffset, uint8_t* pBodyDataBuffer, uint32_t unBufferSize) = 0;
	virtual bool ReleaseHTTPRequest(HTTPRequestHandle hRequest) = 0;
	virtual bool GetHTTPDownloadProgressPct(HTTPRequestHandle hRequest, float* pflPercentOut) = 0;
	virtual bool SetHTTPRequestRawPostBody(HTTPRequestHandle hRequest, const char* pchContentType, uint8_t* pubBody, uint32_t unBodyLen) = 0;
};

ISteamHTTP* s_steam_http;

#pragma pack(push, 8)

struct HTTPRequestCompleted_t
{
	enum { k_iCallback = 2101 };
	HTTPRequestHandle m_hRequest;
	uint64_t m_ulContextValue;
	bool m_bRequestSuccessful;
	int m_eStatusCode;
	uint32_t m_unBodySize;
};

#pragma pack(pop)

struct SteamContext
{
	void* __pad[11];
	ISteamHTTP* steam_http;
};

class VersionCheckCallback
{
public:
	virtual void Run(HTTPRequestCompleted_t *pvParam);
	virtual void Run(HTTPRequestCompleted_t *pvParam, bool bIOFailure, SteamAPICall_t hSteamAPICall) { Run(pvParam); };
	virtual int GetCallbackSizeBytes() { return sizeof(HTTPRequestCompleted_t); }
protected:
	uint8_t m_nCallbackFlags = 0;
	int m_iCallback = HTTPRequestCompleted_t::k_iCallback;
};

static VersionCheckCallback s_check_callback;

bool g_update_needed = false;
std::vector<Commit_t> g_commits_since_compile;

void from_json(const json& j, Commit_t& commit)
{
	commit.author = j["commit"]["author"]["name"].get<std::string>();
	commit.date = j["commit"]["author"]["date"].get<std::string>();
	commit.message = j["commit"]["message"].get<std::string>();
}

void VersionCheckCallback::Run(HTTPRequestCompleted_t* pvParam)
{
	if(pvParam->m_eStatusCode == 200 && pvParam->m_unBodySize > 50)
	{
		g_update_needed = true;

		// Sick regex
		//static const std::regex iso8601(R"("([\+-]?\d{4}(?!\d{2}\b))((-?)((0[1-9]|1[0-2])(\3([12]\d|0[1-9]|3[01]))?|W([0-4]\d|5[0-2])(-?[1-7])?|(00[1-9]|0[1-9]\d|[12]\d{2}|3([0-5]\d|6[1-6])))([T\s]((([01]\d|2[0-3])((:?)[0-5]\d)?|24\:?00)([\.,]\d+(?!:))?)?(\17[0-5]\d([\.,]\d+)?)?([zZ]|([\+-])([01]\d|2[0-3]):?([0-5]\d)?)?)?)?")");
		//static const std::regex iso8601(R"("(\d{4}\-\d\d\-\d\d([tT][\d:\.]*)?)([zZ]|([+\-])(\d\d):?(\d\d))?")");

		std::string body;

		body.resize(pvParam->m_unBodySize + 1);

		auto body_cstr = reinterpret_cast<uint8_t*>(const_cast<char*>(body.data()));

		s_steam_http->GetHTTPResponseBodyData(pvParam->m_hRequest, body_cstr, pvParam->m_unBodySize);

		body_cstr[pvParam->m_unBodySize] = 0;

		try
		{
			g_commits_since_compile = json::parse(body).get<std::vector<Commit_t>>();
		}
		catch(const std::exception&)
		{
			g_update_needed = false;
		}
	}

	s_steam_http->ReleaseHTTPRequest(pvParam->m_hRequest);
}

static void SteamAPI_RegisterCallResult(VersionCheckCallback* callback, SteamAPICall_t handle);

void CheckUpdate()
{
	auto sig_result = platform::FindPattern("client.dll", "\xFF\x15\x00\x00\x00\x00\xB9\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x6A\x00", "xx????x????x????xx");

	if(!sig_result)
		return;

	auto steam_context = *reinterpret_cast<SteamContext**>(sig_result + 7);

	s_steam_http = steam_context->steam_http;

	auto handle = s_steam_http->CreateHTTPRequest(EHTTPMethod::GET, "http://api.github.com/repos/namazso/nSkinz/commits");

	constexpr static auto build_time = time_t(cx::unix_time());

	char build_time_str[32];

	tm build_time_tm;

	gmtime_s(&build_time_tm, &build_time);

	strftime(build_time_str, sizeof build_time_str, "%FT%TZ", &build_time_tm);

	s_steam_http->SetHTTPRequestGetOrPostParameter(handle, "since", build_time_str);

	SteamAPICall_t api_call;

	s_steam_http->SendHTTPRequest(handle, &api_call);

	SteamAPI_RegisterCallResult(&s_check_callback, api_call);
}

#include <windows.h>

void SteamAPI_RegisterCallResult(VersionCheckCallback* callback, SteamAPICall_t handle)
{
	reinterpret_cast<void(__cdecl*)(VersionCheckCallback*, SteamAPICall_t)>
		(GetProcAddress(GetModuleHandleA("steam_api.dll"), "SteamAPI_RegisterCallResult"))(callback, handle);
}