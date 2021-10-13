#include "pch.h"
#include "tipsware.h"
#include <time.h>
#include <stdlib.h>
#include <wininet.h> 
#pragma comment (lib, "wininet.lib")

char titles[300][100];
char address[100] = "board/lists/?id=hiphop&page=1&exception_mode=recommend";
char stopwords[1000][20];
int idx = -1, stop_idx, key_idx = -1;

typedef struct keyword {
	char word[100];
	int count = 0;
} keyword;
keyword keywords[1000];

void set_stop() {
	int i = -1;
	strcpy(stopwords[++i], "아하");
	strcpy(stopwords[++i], "오호");
	strcpy(stopwords[++i], "어허");
	strcpy(stopwords[++i], "하나");
	strcpy(stopwords[++i], "둘");
	strcpy(stopwords[++i], "셋");
	strcpy(stopwords[++i], "넷");
	strcpy(stopwords[++i], "다섯");
	strcpy(stopwords[++i], "여섯");
	strcpy(stopwords[++i], "일곱");
	strcpy(stopwords[++i], "여덟");
	strcpy(stopwords[++i], "아홉");
	strcpy(stopwords[++i], "열");
	strcpy(stopwords[++i], "열하나");
	strcpy(stopwords[++i], "열둘");
	strcpy(stopwords[++i], "열셋");
	strcpy(stopwords[++i], "열넷");
	strcpy(stopwords[++i], "열다섯");
	strcpy(stopwords[++i], "열여섯");
	strcpy(stopwords[++i], "열일곱");
	strcpy(stopwords[++i], "열여덟");
	strcpy(stopwords[++i], "열아홉");
	strcpy(stopwords[++i], "스물");
	strcpy(stopwords[++i], "우리");
	strcpy(stopwords[++i], "저희");
	strcpy(stopwords[++i], "따라");
	strcpy(stopwords[++i], "의해");
	strcpy(stopwords[++i], "에게");
	strcpy(stopwords[++i], "래퍼");
	strcpy(stopwords[++i], "의거");
	strcpy(stopwords[++i], "의거하여");
	strcpy(stopwords[++i], "때문에");
	strcpy(stopwords[++i], "때문");
	strcpy(stopwords[++i], "하지마");
	strcpy(stopwords[++i], "해줘");
	strcpy(stopwords[++i], "먹었다");
	strcpy(stopwords[++i], "받았다");
	strcpy(stopwords[++i], "이쯤에서");
	strcpy(stopwords[++i], "취향인");
	strcpy(stopwords[++i], "취향");
	strcpy(stopwords[++i], "빠는");
	strcpy(stopwords[++i], "애들");
	strcpy(stopwords[++i], "프로");
	strcpy(stopwords[++i], "확률로");
	strcpy(stopwords[++i], "있음");
	strcpy(stopwords[++i], "없음");
	strcpy(stopwords[++i], "있냐");
	strcpy(stopwords[++i], "없냐");
	strcpy(stopwords[++i], "보자");
	strcpy(stopwords[++i], "저격");
	strcpy(stopwords[++i], "사실");
	strcpy(stopwords[++i], "아니");
	strcpy(stopwords[++i], "거임");
	strcpy(stopwords[++i], "거야");
	strcpy(stopwords[++i], "유난히");
	strcpy(stopwords[++i], "생각");
	strcpy(stopwords[++i], "해봤는데");
	strcpy(stopwords[++i], "들어봤는데");
	strcpy(stopwords[++i], "별로");
	strcpy(stopwords[++i], "예고");
	strcpy(stopwords[++i], "해라");
	strcpy(stopwords[++i], "이유");
	strcpy(stopwords[++i], "인스타");
	strcpy(stopwords[++i], "스토리");
	strcpy(stopwords[++i], "라방");
	strcpy(stopwords[++i], "페북");
	strcpy(stopwords[++i], "이건");
	strcpy(stopwords[++i], "진짜");
	strcpy(stopwords[++i], "가능");
	strcpy(stopwords[++i], "신곡");
	strcpy(stopwords[++i], "오늘");
	strcpy(stopwords[++i], "내일");
	strcpy(stopwords[++i], "모레");
	strcpy(stopwords[++i], "sns");
	strcpy(stopwords[++i], "jpg");
	strcpy(stopwords[++i], "jp");
	strcpy(stopwords[++i], "gif");
	strcpy(stopwords[++i], "avi");
	strcpy(stopwords[++i], "av");
	strcpy(stopwords[++i], "mp4");
	strcpy(stopwords[++i], "mp");
	strcpy(stopwords[++i], "txt");
	strcpy(stopwords[++i], "tx");
	strcpy(stopwords[++i], "feat");
	strcpy(stopwords[++i], "remix");
	strcpy(stopwords[++i], "그리고");
	strcpy(stopwords[++i], "그러나");
	strcpy(stopwords[++i], "비록");
	strcpy(stopwords[++i], "더라도");
	strcpy(stopwords[++i], "아니면");
	strcpy(stopwords[++i], "제외하고");
	strcpy(stopwords[++i], "비로소");
	strcpy(stopwords[++i], "부터");
	strcpy(stopwords[++i], "까지");
	strcpy(stopwords[++i], "이랑");
	strcpy(stopwords[++i], "엄마");
	strcpy(stopwords[++i], "아빠");
	strcpy(stopwords[++i], "애미");
	strcpy(stopwords[++i], "애비");
	strcpy(stopwords[++i], "대해서");
	strcpy(stopwords[++i], "남짓");
	strcpy(stopwords[++i], "약간");
	strcpy(stopwords[++i], "다소");
	strcpy(stopwords[++i], "대박");
	strcpy(stopwords[++i], "어느정도");
	strcpy(stopwords[++i], "대부분");
	strcpy(stopwords[++i], "거의");
	strcpy(stopwords[++i], "당장");
	strcpy(stopwords[++i], "결국");
	strcpy(stopwords[++i], "차라리");
	strcpy(stopwords[++i], "제발");
	strcpy(stopwords[++i], "애초에");
	strcpy(stopwords[++i], "해요");
	strcpy(stopwords[++i], "다음");
	strcpy(stopwords[++i], "어때");
	strcpy(stopwords[++i], "녹음");
	strcpy(stopwords[++i], "해주세요");
	strcpy(stopwords[++i], "해줘");
	strcpy(stopwords[++i], "할까");
	strcpy(stopwords[++i], "하고싶다");
	strcpy(stopwords[++i], "사람");
	strcpy(stopwords[++i], "이기");
	strcpy(stopwords[++i], "이새끼");
	strcpy(stopwords[++i], "이랑");
	strcpy(stopwords[++i], "여서");
	strcpy(stopwords[++i], "냠");
	strcpy(stopwords[++i], "힙붕이");
	strcpy(stopwords[++i], "나눈");
	strcpy(stopwords[++i], "존나");
	strcpy(stopwords[++i], "노래");
	strcpy(stopwords[++i], "힙붕");
	strcpy(stopwords[++i], "힙합");
	strcpy(stopwords[++i], "힙갤");
	strcpy(stopwords[++i], "국힙");
	strcpy(stopwords[++i], "와꾸");
	strcpy(stopwords[++i], "솔직히");
	strcpy(stopwords[++i], "민심");
	strcpy(stopwords[++i], "테스트");
	strcpy(stopwords[++i], "솔직하게");
	strcpy(stopwords[++i], "소신발언");
	strcpy(stopwords[++i], "잼민이");
	strcpy(stopwords[++i], "잼민이들");
	strcpy(stopwords[++i], "대해");
	strcpy(stopwords[++i], "충격");
	strcpy(stopwords[++i], "다이어트");
	strcpy(stopwords[++i], "비교");
	strcpy(stopwords[++i], "확실히");
	strcpy(stopwords[++i], "확실하게");
	strcpy(stopwords[++i], "하네");
	strcpy(stopwords[++i], "힙알못");
	strcpy(stopwords[++i], "힙잘알");
	strcpy(stopwords[++i], "후기");
	strcpy(stopwords[++i], "속보");
	strcpy(stopwords[++i], "내가");
	strcpy(stopwords[++i], "친분");
	strcpy(stopwords[++i], "인맥");
	strcpy(stopwords[++i], "근황");
	strcpy(stopwords[++i], "실시간");
	strcpy(stopwords[++i], "오늘자");
	strcpy(stopwords[++i], "ㄹㅇ");
	strcpy(stopwords[++i], "이거");
	strcpy(stopwords[++i], "저거");
	strcpy(stopwords[++i], "이게");
	strcpy(stopwords[++i], "저게");
	strcpy(stopwords[++i], "까이");
	strcpy(stopwords[++i], "뭐냐");
	strcpy(stopwords[++i], "예고");
	strcpy(stopwords[++i], "빠르고");
	strcpy(stopwords[++i], "빡센");
	strcpy(stopwords[++i], "아깝긴");
	strcpy(stopwords[++i], "소름인");
	strcpy(stopwords[++i], "장문");
	strcpy(stopwords[++i], "논란");
	strcpy(stopwords[++i], "한테");
	strcpy(stopwords[++i], "대체");
	strcpy(stopwords[++i], "좀");
	strcpy(stopwords[++i], "등");
	strcpy(stopwords[++i], "해");
	strcpy(stopwords[++i], "살");
	strcpy(stopwords[++i], "휴");
	strcpy(stopwords[++i], "뿐");
	strcpy(stopwords[++i], "쫌");
	strcpy(stopwords[++i], "즉");
	strcpy(stopwords[++i], "퍽");
	strcpy(stopwords[++i], "팍");
	strcpy(stopwords[++i], "헉");
	strcpy(stopwords[++i], "퉤");
	strcpy(stopwords[++i], "쳇");
	strcpy(stopwords[++i], "점");
	strcpy(stopwords[++i], "또");
	strcpy(stopwords[++i], "곧");
	stop_idx = i;
}

void set_clear() { // 전처리 과정, 소문자 대문자로 바꾸고 숫자 없애자
	for (int i = 0; i <= idx; i++) {
		for (int j = 0; j < strlen(titles[i]); j++) {
			if ('A' <= titles[i][j] && titles[i][j] <= 'Z')
				titles[i][j] += 'a' - 'A';
			else if ('0' <= titles[i][j] && titles[i][j] <= '9')
				titles[i][j] = ' ';
			else if (titles[i][j] == '-' || titles[i][j] == '★' || titles[i][j] == '=' || titles[i][j] == '#')
				titles[i][j] = ' ';
			else if (titles[i][j] == '^' || titles[i][j] == '*' || titles[i][j] == '?' || titles[i][j] == '!' || titles[i][j] == '~' || titles[i][j] == '.')
				titles[i][j] = ' ';
			else if (titles[i][j] == ')' || titles[i][j] == '(')
				titles[i][j] = ' ';
			else {
				char temp[10];
				temp[0] = titles[i][j];
				temp[1] = titles[i][j + 1];
				temp[2] = '\0';

				if (!strcmp(temp, "가") || !strcmp(temp, "이") || !strcmp(temp, "는") || !strcmp(temp, "은") || !strcmp(temp, "를") || !strcmp(temp, "을") || !strcmp(temp, "도") || !strcmp(temp, "와") || !strcmp(temp, "과")|| !strcmp(temp, "다")) {
					if(j < strlen(titles[i]) - 2)
						if (titles[i][j + 2] == ' ') {
							titles[i][j] = ' ';
							titles[i][j + 1] = ' ';
						}
				}
				else if (!strcmp(temp, "ㄱ") || !strcmp(temp, "ㄴ") || !strcmp(temp, "ㄷ") || !strcmp(temp, "ㅋ") || !strcmp(temp, "ㄹ") || !strcmp(temp, "ㅇ") || !strcmp(temp, "ㅈ") || !strcmp(temp, "ㅉ")) {
					titles[i][j] = ' ';
					titles[i][j + 1] = ' ';
				}
			}
		}
	}
}

void set_keyword() {
	char temp[100];
	char* cpo;

	for (int i = 0; i <= idx; i++) {
		for (int j = 0; j <= stop_idx; j++) { // 먼저 불용어들 모두 space로 치환
			cpo = strstr(titles[i], stopwords[j]);
			if (cpo) {
				for (int k = 0; k < strlen(stopwords[j]); k++)
					cpo[k] = ' ';
			}
		}

		int flag = 0, t = -1;
		for (int j = 0; j < strlen(titles[i]); j++) { // 남는 단어들 모두 keyword에 추가
			if (titles[i][j] == '<')
				break;
			if (titles[i][j] == ' ' || titles[i][j] == '\0') {
				if (flag) {
					temp[++t] = '\0';
					flag = 0;
					t = -1;

					int find = -1;
					for (int k = 0; k <= key_idx; k++)
						if (!strcmp(keywords[k].word, temp)) {
							find = k;
							break;
						}
					if (find == -1) { // 새로운 키워드 추가
						strcpy(keywords[++key_idx].word, temp);
						keywords[key_idx].count = 1;
					}
					else
						keywords[find].count++;
				}
			}
			else {
				flag = 1;
				temp[++t] = titles[i][j];
			}
		}
	}
}

void sort_keyword() {
	int a, b, c, d;

	for (int i = 0; i <= key_idx; i++) { // 쓰레기 단어들 다 -1 할당
		int len = strlen(keywords[i].word);
		if (len <= 3) {
			keywords[i].count = -1;
		}
		if (!strcmp(keywords[i].word, "힙뀽")) {
			a = i;
		}
		if (!strcmp(keywords[i].word, "힙뀽이")) {
			b = i;
		}
		if (!strcmp(keywords[i].word, "젓딧")) {
			c = i;
		}
		if (!strcmp(keywords[i].word, "저스디스")) {
			d = i;
		}
	}
	keywords[b].count += keywords[a].count;
	keywords[a].count = -1;
	keywords[d].count += keywords[c].count;
	keywords[c].count = -1;

	for(int i = 0; i <= key_idx; i++)
		for (int j = 0; j <= key_idx - i - 1; j++) {
			if (keywords[j].count < keywords[j + 1].count) {
				int temp = keywords[j].count;
				keywords[j].count = keywords[j + 1].count;
				keywords[j + 1].count = temp;

				char tem[100];
				strcpy(tem, keywords[j].word);
				strcpy(keywords[j].word, keywords[j + 1].word);
				strcpy(keywords[j + 1].word, tem);
			}
		}

}

DWORD ReadHtmlText(HINTERNET ah_http_file, char *ap_html_string)
{
	char buffer[1025];
	DWORD read_byte = 0, error_count = 0, total_bytes = 0;
	while (InternetReadFile(ah_http_file, buffer, 1024, &read_byte)) {
		memcpy(ap_html_string + total_bytes, buffer, read_byte);
		total_bytes += read_byte;
		if (read_byte < 1024) {
			error_count++;
			if (error_count > 10) break;
			else Sleep(5); // 5ms 지연
		}
		else error_count = 0;
	}

	*(ap_html_string + total_bytes) = 0;
	return total_bytes;
}

void LoadDataFromWebPage()
{
	HINTERNET h_session = InternetOpen("Hiphop", PRE_CONFIG_INTERNET_ACCESS, NULL, INTERNET_INVALID_PORT_NUMBER, 0);
	HINTERNET h_connect = InternetConnect(h_session, "gall.dcinside.com", INTERNET_INVALID_PORT_NUMBER, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
	HINTERNET h_http_file = HttpOpenRequest(h_connect, "GET", address, HTTP_VERSION, NULL, 0, INTERNET_FLAG_DONT_CACHE, 0);

	char *p_utf8_html_str = (char *)malloc(1024 * 1024);
	if (p_utf8_html_str != NULL) {
		if (HttpSendRequest(h_http_file, NULL, 0, 0, 0) == TRUE) {

			ReadHtmlText(h_http_file, p_utf8_html_str);

			char *p_ascii_html_str = NULL;

			Utf8ToAscii(&p_ascii_html_str, p_utf8_html_str);
			if (p_ascii_html_str != NULL) {
				SetCtrlName(FindControl(1000), p_ascii_html_str);
				free(p_ascii_html_str);
			}
		}
		free(p_utf8_html_str);
	}

	if (h_http_file != NULL) InternetCloseHandle(h_http_file);
	if (h_connect != NULL) InternetCloseHandle(h_connect);
	if (h_session != NULL) InternetCloseHandle(h_session);
}

char *CopyTextFromWebData(char *ap_dest_str, char *ap_src_str, const char *ap_start_word, const char *ap_end_word)
{
	char *p_pos = strstr(ap_src_str, ap_start_word);
	if (p_pos != NULL) {
		char *p_limit_pos = strstr(p_pos, ap_end_word);
		if (p_limit_pos != NULL) {
			p_pos += strlen(ap_start_word);
			while (p_pos < p_limit_pos) {
				*ap_dest_str++ = *p_pos++;
			}
			*ap_dest_str = 0;
			return p_pos;
		}
	}
	return ap_src_str; 
}


void ShowRank()
{
	for (int i = 0; i < 3; i++) { // 3 page 까지 읽기
		switch (i) {
		case 0 :
			address[28] = '1';
			break;
		case 1:
			address[28] = '2';
			break;
		case 2:
			address[28] = '3';
			break;
		}

		LoadDataFromWebPage(); // html 가져오기

		void *p_edit = FindControl(1000);
		int len = Edit_GetLength(p_edit) + 1;
		char time_str[1000];
		char *p_html_str = (char *)malloc(len);
		if (p_html_str != NULL) {
			GetCtrlName(p_edit, p_html_str, len);

			char *p_pos;
			do {
				p_pos = strstr(p_html_str, "view-msg =\"\">");
				if (p_pos != NULL) {
					p_pos[0] = 'x';
					p_pos += 12;

					p_pos = CopyTextFromWebData(time_str, p_pos, "</em>", "</a>");
					strcpy(titles[++idx], time_str);
				}
			} while (p_pos != NULL); // 글 제목만 titles에 저장

			free(p_html_str);
		}
	}

	set_clear(); // 전처리 과정
	set_keyword(); // stopwords 제외하고 키워드만 추출
	sort_keyword(); // 의미없는 키워드 제외, 정렬
	int asd = 0;
	
	for (int j = 1; j <= 20; j++) {
		SelectFontObject("Consolas", 18, 1);
		TextOut(100, 115 + (++asd) * 20, RGB(200, 240, 250), "%2d : ", j);
		SelectFontObject("나눔고딕", 18, 1);
		TextOut(210, 115 + (asd) * 20, RGB(240, 255, 160), "%8s", keywords[j].word);
		SelectFontObject("나눔고딕", 12, 1);
		TextOut(430, 115 + (asd) * 20, RGB(150, 180, 160), "%2d회", keywords[j].count);
	}
		
	ShowDisplay();
}

void OnCommand(INT32 a_ctrl_id, INT32 a_notify_code, void *ap_ctrl)
{
	//if (a_ctrl_id == 1011) ShowRank();
}

CMD_MESSAGE(OnCommand)

int main()
{
	ChangeWorkSize(500, 600);
	Clear(0, RGB(82, 97, 124));


	CreateEdit(5, 5, 490, 100, 1000, ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_MULTILINE | ES_WANTRETURN | WS_VSCROLL | WS_HSCROLL);

	CreateButton("랭킹 새로고침", 150, 560, 200, 28, 1011);

	ShowDisplay();

	set_stop();
	ShowRank();

	return 0;
}