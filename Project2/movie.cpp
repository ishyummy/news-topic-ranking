#include "pch.h"
#include "tipsware.h"

// HTTP 프로토콜을 사용하는 함수들이 정의된 wininet.h 헤더 파일을 추가한다.
#include <wininet.h> 
// wininet.h 헤더에 정의된 함수들을 사용하려면 wininet.lib 라이브러리를 추가해야 한다.
#pragma comment (lib, "wininet.lib")

struct MovieData
{
	char ico_rate[16];             // 관람 등급
	char title[64];                // 제목
	char want[12];                 // 좋아요
	char dont_want[12];            // 글쎄요
	char genre[128];               // 장르
	char etc[32];                  // 상영 정보
	char director[128];            // 감독
	char actor[256];               // 연기자
	unsigned short ico_rate_len;   // 관람 등급 문자열 길이
	unsigned short title_len;      // 제목 문자열 길이
	unsigned short want_len;       // 좋아요 문자열 길이
	unsigned short dont_want_len;  // 글쎄요 문자열 길이
	unsigned short genre_len;      // 장르 문자열 길이
	unsigned short director_len;   // 감독 문자열 길이
	unsigned short actor_len;      // 연기자 문자열 길이
	unsigned short etc_len;        // 상영 정보 문자열 길이
};

DWORD ReadHtmlText(HINTERNET ah_http_file, char *ap_html_string)
{
	// 잠시 지연 효과를 메인 스레드에게만 사용하기 위해 이벤트 객체를 생성한다.
	HANDLE h_wait_event = CreateEvent(NULL, TRUE, 0, NULL);
	if (h_wait_event == NULL) return 0;  // 이벤트 객체 생성에 실패한 경우

	char buffer[1025];
	DWORD read_byte = 0, error_count = 0, total_bytes = 0;
	// 1024 바이트 단위로 HTML 코드를 가져옴
	while (InternetReadFile(ah_http_file, buffer, 1024, &read_byte)) {
		// 1024 단위로 가져온 HTML 소스를 ap_html_string에 계속 추가한다.
		memcpy(ap_html_string + total_bytes, buffer, read_byte);
		total_bytes += read_byte;  // 읽은 전체 바이트 수를 갱신한다.
		if (read_byte < 1024) {
			// 1024바이트씩 요청하더라도 해당 페이지의 HTML 소스가 1024보다 작거나
			// 네트워크 지연으로 더 작은 크기가 전송될수 있음. (10번 정도 재시도함)
			error_count++;
			if (error_count > 10) break;
			else WaitForSingleObject(h_wait_event, 5); // 5ms 지연하도록 구성한다.
		}
		else error_count = 0;
	}

	*(ap_html_string + total_bytes) = 0;  // 문자열의 끝에 NULL 문자를 추가함!
	CloseHandle(h_wait_event);  // 지연 효과를 위해 생성한 이벤트 객체를 제거한다.
	return total_bytes;  // 읽어들인 전체 바이트 수를 반환한다.
}

// 분석에 의미없는 문자들을 제거하는 함수!
void RemoveMeaninglessChar(char *ap_string)
{
	// 탭, 줄바꿈 문자가 나올때까지 반복한다.
	while (*ap_string) {
		if (*ap_string == '\t' || *ap_string == '\r' || *ap_string == '\n') break;
		ap_string++;
	}
	if (!*ap_string) return;  // 전체적으로 탭, 줄바꿈 문자가 없는 경우!

	char *p_dest = ap_string++;
	while (*ap_string) {
		// 탭, 줄바꿈 문자가 아닌 경우에만 복사를 진행한다.
		if (*ap_string != '\t' && *ap_string != '\r' && *ap_string != '\n') {
			*p_dest++ = *ap_string;
		}
		ap_string++;
	}
	*p_dest = 0;  // 문자열의 끝에 NULL 문자를 추가한다.
}

void LoadDataFromWebPage()  // 웹 페이지를 구성하는 HTML 소스를 가져오는 함수
{
	// 인터넷을 사용할 세션을 구성한다.
	HINTERNET h_session = InternetOpen("MovieScanner", PRE_CONFIG_INTERNET_ACCESS, NULL, INTERNET_INVALID_PORT_NUMBER, 0);
	// 어떤 사이트에 접속할지 구성한다. (네이버 영화를 사용하도록 구성한다.)
	HINTERNET h_connect = InternetConnect(h_session, "movie.naver.com", INTERNET_INVALID_PORT_NUMBER, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
	// 네이버 영화에서 개봉 예정영화 페이지에 접속한다. (url 배열에 저장된 페이지를 연다.)
	HINTERNET h_http_file = HttpOpenRequest(h_connect, "GET", "/movie/running/premovie.nhn", HTTP_VERSION, NULL, 0, INTERNET_FLAG_DONT_CACHE, 0);

	char *p_utf8_html_str = (char *)malloc(2 * 1024 * 1024);  // 2M Bytes 메모리 할당!
	if (p_utf8_html_str != NULL) {  // 메모리 할당에 성공했다면!
		if (HttpSendRequest(h_http_file, NULL, 0, 0, 0) == TRUE) { // 웹 페이지 소스를 요청한다.
			// 웹 페이지 전체 소스를 할당된 메모리에 복사한다.
			ReadHtmlText(h_http_file, p_utf8_html_str);
			// HTML 소스 분석에 영향을 미치지 않는 탭, 줄바꿈 문자를 제거한다.
			RemoveMeaninglessChar(p_utf8_html_str);

			char *p_ascii_html_str = NULL;
			// 가져온 웹 페이지 소스가 UTF8 형식의 문자열이라서 ASCII 형식의 문자열로 변환합니다.
			Utf8ToAscii(&p_ascii_html_str, p_utf8_html_str);
			if (p_ascii_html_str != NULL) {  // 변환에 성공했다면
				// 변환된 HTML 소스를 1000번 에디트 컨트롤에 표시한다.
				SetCtrlName(FindControl(1000), p_ascii_html_str);
				free(p_ascii_html_str);  // 변환에 사용했던 메모리를 해제한다.
			}
		}
		free(p_utf8_html_str);  // 웹 페이지 소스를 저장하기 위해 할당했던 메모리를 해제한다.
	}

	// 웹 페이지를 사용하기 위해 할당했던 핸들을 모두 해제한다.
	if (h_http_file != NULL) InternetCloseHandle(h_http_file);
	if (h_connect != NULL) InternetCloseHandle(h_connect);
	if (h_session != NULL) InternetCloseHandle(h_session);
}

void DestoryMovieData()
{
	void *p_data, *p_ctrl = FindControl(1001);  // 사용자 목록이 저장된 리스트 박스의 주소를 얻는다.
	int count = ListBox_GetCount(p_ctrl); // 리스트 박스에 추가된 항목의 수를 얻는다.  
	for (int i = 0; i < count; i++) {  // 리스트 박스의 각 항목에 추가된 메모리를 모두 제거한다.
		p_data = ListBox_GetItemDataPtr(p_ctrl, i);  // i 번째 항목에 저장된 주소를 가져온다.
		free(p_data); // p_data가 가리키는 메모리를 해제한다.
	}
	ListBox_ResetContent(p_ctrl);  // 리스트 박스의 모든 항목을 제거한다.
}

// "> 문자열 뒤부터 < 문자가 나올때까지 문자열을 복사하는 함수! 복사한 문자열의 길이도 알려줌!
char *CopyStringFromWebData(char *ap_dest_str, unsigned short *ap_dest_len, char *ap_src_str)
{
	ap_src_str = strstr(ap_src_str, "\">");  // "> 문자열 위치를 찾는다.
	if (ap_src_str != NULL) {  // 찾았다면!
		char *p_start_pos = ap_dest_str;  // 탐색전에 복사할 메모리의 시작 위치를 기억한다.
		ap_src_str += 2;  // "> 문자열은 제외한다.
		// < 문자가 나올때까지 문자열을 복사한다.
		while (*ap_src_str && *ap_src_str != '<') *ap_dest_str++ = *ap_src_str++;
		*ap_dest_str = 0;  // NULL 문자를 추가한다.
		*ap_dest_len = ap_dest_str - p_start_pos;  // 문자열의 길이를 계산해서 대입한다.
	}
	return ap_src_str;  // 작업에 실패했으면 전달받은 위치를 그대로 반환한다.
}

// 영화 페이지에 있는 LinkText 항목을 기준으로 문자열을 복사할 때 사용하는 함수!
char *CopyLinkTextFromWebData(char *ap_dest_str, unsigned short *ap_dest_len, char *ap_src_str)
{
	char *p_limit_pos = strstr(ap_src_str, "</span>");   // </span> 문자열을 사용해서 마지막 위치를 찾는다.
	if (p_limit_pos != NULL) {  // 찾았다면!
		char *p_start_pos = ap_dest_str, open_flag = 0;
		//  <  > 로 포함된 문자열은 제외하고 나머지 문자열만 복사한다.
		while (ap_src_str < p_limit_pos) {
			if (*ap_src_str == '<') open_flag = 1;
			else if (*ap_src_str == '>') open_flag = 0;
			else {
				if (!open_flag) *ap_dest_str++ = *ap_src_str;
			}
			ap_src_str++;
		}
		*ap_dest_str = 0;  // NULL 문자를 추가한다.
		*ap_dest_len = ap_dest_str - p_start_pos;  // 문자열의 길이를 계산해서 대입한다.
		return p_limit_pos + 7;  // +7은 </span> 길이만큼 지나감!
	}
	return ap_src_str; // 작업에 실패했으면 전달받은 위치를 그대로 반환한다.
}

// 상영 시간과 개봉 일자를 읽을 때 사용하는 함수!
char *CopyEtcDataFromWebData(char *ap_dest_str, unsigned short *ap_dest_len, char *ap_src_str)
{
	char *p_limit_pos = strstr(ap_src_str, "</dd>");  // </dd> 문자열을 사용해서 마지막 위치를 찾는다.
	if (p_limit_pos != NULL) {  // 찾았다면!
		char *p_start_pos = ap_dest_str, open_flag = 0;
		//  <  > 로 포함된 문자열은 제외하고 나머지 문자열만 복사한다.
		while (ap_src_str < p_limit_pos) {
			if (*ap_src_str == '<') open_flag = 1;
			else if (*ap_src_str == '>') open_flag = 0;
			else {
				if (!open_flag) {
					// 중간에 '|' 문자를 만나면 ", " 문자열로 변경해서 추가한다.
					if (*ap_src_str == '|') {
						if (p_start_pos != ap_dest_str) {
							*ap_dest_str++ = ',';
							*ap_dest_str++ = ' ';
						}
					}
					else *ap_dest_str++ = *ap_src_str;
				}
			}
			ap_src_str++;
		}
		*ap_dest_str = 0;  // NULL 문자를 추가한다.
		*ap_dest_len = ap_dest_str - p_start_pos; // 문자열의 길이를 계산해서 대입한다.
		return p_limit_pos + 5;  // +5는 </dd> 길이만큼 지나감!
	}
	return ap_src_str;
}


char *AddMovieInfo(char *ap_string)
{
	// 상영작 정보를 저장하기 위한 메모리를 할당한다!
	MovieData *p_movie_data = (MovieData *)malloc(sizeof(MovieData));
	if (p_movie_data == NULL) return NULL;  // 메모리 시스템 오류 발생!!

	memset(p_movie_data, 0, sizeof(MovieData));  // 할당된 메모리를 초기화한다.
	void *p_movie_list = FindControl(1001);   // 영화 목록을 저장할 리스트 박스의 주소를 얻는다.
	char *p_pos = strstr(ap_string, "<span class=\"");  // 각 항목 정보들은 <span class=" 로 구분된다!

	while (p_pos != NULL) {
		p_pos += 13;  // + 13는 <span class=" 길이 만큼 지나감!

		if (!strncmp(p_pos, "ico_rating_", 11)) {  // 관람 기준 정보인 경우!
			p_pos = CopyStringFromWebData(p_movie_data->ico_rate, &p_movie_data->ico_rate_len, p_pos + 11);  // + 11는 "ico_rating 길이 만큼 지나감
			p_pos = CopyStringFromWebData(p_movie_data->title, &p_movie_data->title_len, p_pos + 7);  // +7은 </span> 길이 만큼 지나감
		}
		else if (!strncmp(p_pos, "num", 3)) {  // 예매율!
		 // 현재 사용 안함! 나중에 필요하면 구현하세요!
			p_pos = strstr(p_pos, "</dd>"); // </dd> 까지 지나감!
		}
		else if (!strncmp(p_pos, "ico_want", 8)) {  // 기대지수
			p_pos = strstr(p_pos + 8, "\"exp_cnt\"");  // +8은 ico_want 길이 만큼 지나감
			if (p_pos == NULL) break;
			p_pos = CopyStringFromWebData(p_movie_data->want, &p_movie_data->want_len, p_pos + 8);  // +8은 "exp_cnt 길이 만큼 지나감
			p_pos = strstr(p_pos, "\"exp_cnt\"");
			p_pos = CopyStringFromWebData(p_movie_data->dont_want, &p_movie_data->dont_want_len, p_pos + 8);  // +8은 "exp_cnt 길이 만큼 지나감
			p_pos += 25;  //  기대 지수 뒤에 있는 문자열 지나감!
		}
		else if (!strncmp(p_pos, "link_txt", 8)) {  // 영화 장르, 감독 또는 연기자!
			p_pos += 10;  // +10은 "link_text"> 길이 만큼 지나감
			if (!strncmp(p_pos - 36, "개요", 4)) {
				p_pos = CopyLinkTextFromWebData(p_movie_data->genre, &p_movie_data->genre_len, p_pos);  // 장르 정보!
				p_pos = CopyEtcDataFromWebData(p_movie_data->etc, &p_movie_data->etc_len, p_pos);  // 상영 시간과 개봉일자!
			}
			else if (!strncmp(p_pos - 36, "감독", 4)) {
				p_pos = CopyLinkTextFromWebData(p_movie_data->director, &p_movie_data->director_len, p_pos);  // 감독 정보!
			}
			else {  // 출연진!
				p_pos = CopyLinkTextFromWebData(p_movie_data->actor, &p_movie_data->actor_len, p_pos);  // 출연진 정보!!
			}
		}
		else if (!strncmp(p_pos, "btn_", 4)) {
			// p_movie_data에 구성된 영화 정보를 리스트 박스에 추가한다.
			ListBox_SetItemDataPtrEx(p_movie_list, -1, "", p_movie_data, 0);
			break;  // 영화 한 편에 대한 정보가 끝났음
		}
		p_pos = strstr(p_pos, "<span class=\"");  // 각 항목 정보들은 <span class=" 로 구분된다!
	}

	if (p_movie_data->title_len == 0) {  // 이름 검색에 실패한 경우!!
		// 영화 관람 기준 정보가 없어서 영화 검색에 실패한 경우에는 이름만 다시 추적해서 추가한다.
		ap_string = strstr(ap_string, "<dt class=\"tit\">");
		if (ap_string != NULL) {
			CopyStringFromWebData(p_movie_data->title, &p_movie_data->title_len, ap_string + 16);  // +16은 <dt class="tit"> 길이 만큼 지나감
		}
	}
	return p_pos;  // 작업이 끝난 위치를 반환한다.
}

void ConstructMovieList()  // HTML 정보를 사용해서 영화 목록 구성하기!
{
	DestoryMovieData();  // 리스트 박스에 등록되어 있던 기존 영화 정보를 제거한다!

	void *p_edit = FindControl(1000);  // 에디트 컨트롤의 주소를 얻는다.
	int len = Edit_GetLength(p_edit) + 1;  // 에디트 컨트롤에 저장된 문자열의 길이를 얻는다.
	char *p_html_str = (char *)malloc(len); // 에디트 컨트롤에 저장된 문자열의 길이만큼 메모리 할당!
	if (p_html_str != NULL) {
		GetCtrlName(p_edit, p_html_str, len);  // 에디트 컨트롤에 저장된 문자열을 p_html_str에 복사한다.

		char *p_pos = strstr(p_html_str, "<li><div class=\"thumb\">");  // 영화 목록의 시작 위치를 찾는다!
		char *p_limit_pos = strstr(p_html_str, "<!-- 검색어 순위 -->");  // 영화 목록의 끝 위치를 찾는다!

		while (p_pos != NULL && p_pos < p_limit_pos) {
			p_pos = AddMovieInfo(p_pos + 23);  // + 23은 <li><div class="thumb"> 길이 만큼 지나감!
			if (p_pos == NULL) break;  // 정보가 중단에 끝남!
			p_pos = strstr(p_pos, "<li><div class=\"thumb\">");  // 영화 한 편의 시작 위치를 찾는다!
		}

		free(p_html_str);  // 에디트 컨트롤에 저장된 문자열을 복사하기 위해 할당했던 메모리 해제!
	}
}

// 컨트롤을 조작했을 때 호출할 함수 만들기
// 컨트롤의 아이디(a_ctrl_id), 컨트롤의 조작 상태(a_notify_code), 선택한 컨트롤 객체(ap_ctrl)
void OnCommand(INT32 a_ctrl_id, INT32 a_notify_code, void *ap_ctrl)
{
	if (a_ctrl_id == 1010) LoadDataFromWebPage();  // "영화 페이지 소스 가져오기" 버튼이 눌러진 경우!
	else if (a_ctrl_id == 1011) ConstructMovieList();  // "상영 예정작 보기" 버튼이 눌러진 경우!
}

// 등록된 사용자 목록에 추가된 항목을 출력하는 함수. 이 함수는 ListBox에 추가된 항목 수만큼 호출된다.
void DrawMovieDataItem(int index, char *ap_str, int a_str_len, void *ap_data, int a_is_selected, RECT *ap_rect)
{
	// 리스트 박스의 각 항목이 선택되었을 때와 아닐 때의 항목 테두리 색상을 다르게 처리함!
	if (a_is_selected) SelectPenObject(RGB(200, 255, 255));  // 선택 됨
	else SelectPenObject(RGB(62, 77, 104));  // 선택 안됨

	SelectBrushObject(RGB(62, 77, 104));  // 각 항목의 배경을 색상을 지정한다.
	// 각 항목의 배경으로 사용할 사각형을 그린다.
	Rectangle(ap_rect->left, ap_rect->top, ap_rect->right, ap_rect->bottom);

	MovieData *p_data = (MovieData *)ap_data; // 현재 항목에 연결된 사용자 정보를 사용하기 위해 형변환을 한다.
	SelectFontObject("굴림체", 12);  // 글꼴을 '굴림', 12 크기로 설정한다.

	TextOut(ap_rect->left + 5, ap_rect->top + 5, RGB(100, 100, 200), "%s", p_data->ico_rate);  // 관람 기준 등급
	TextOut(ap_rect->left + 105, ap_rect->top + 5, RGB(0, 200, 255), "%s", p_data->title); // 영화 제목
	TextOut(ap_rect->left + 420, ap_rect->top + 5, RGB(200, 200, 210), "%s", p_data->etc); // 상영 시간과 개봉 일자
	TextOut(ap_rect->left + 5, ap_rect->top + 21, RGB(255, 255, 255), "%s/%s", p_data->want, p_data->dont_want); // 좋아요, 글세요
	TextOut(ap_rect->left + 105, ap_rect->top + 21, RGB(0, 232, 200), "%s", p_data->genre);  // 영화 장르
	TextOut(ap_rect->left + 115 + p_data->genre_len * 6, ap_rect->top + 21, RGB(242, 200, 228), "감독 : %s", p_data->director);  // 감독
	TextOut(ap_rect->left + 5, ap_rect->top + 37, RGB(228, 228, 100), "연기자 : %s", p_data->actor); // 출연진
}

// 컨트롤이 조작되면 OnCommand 함수를 호출하고 프로그램이 종료될 때 DestoryUserData 함수를 호출하도록 설정한다.
CMD_USER_MESSAGE(OnCommand, DestoryMovieData, NULL)

int main()
{
	ChangeWorkSize(600, 500); // 작업 영역을 설정한다.
	Clear(0, RGB(82, 97, 124)); // 작업 영역을 RGB(82, 97, 124) 색으로 채운다!

	// 웹 페이지 소스를 보여줄 에디트 컨트롤을 생성한다.
	// (여러 줄 사용 모드, 가로/세로 자동 스크롤, 수직 스크롤바 사용, 엔터키 사용 모드)
	CreateEdit(5, 5, 590, 200, 1000, ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_MULTILINE | ES_WANTRETURN | WS_VSCROLL | WS_HSCROLL);
	void *p_movie_list = CreateListBox(5, 210, 590, 253, 1001, DrawMovieDataItem); // 리스트 박스를 생성
	ListBox_SetItemHeight(p_movie_list, 53);  // 항목의 높이를 53으로 설정한다.

	CreateButton("영화 페이지 소스 가져오기", 5, 468, 200, 28, 1010);  // 버튼 생성!
	CreateButton("상영 예정작 보기", 210, 468, 200, 28, 1011);  // 버튼 생성!

	ShowDisplay(); // 정보를 윈도우에 출력한다.
	return 0;
}