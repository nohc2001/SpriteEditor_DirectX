#define H_HANCOM
#include <iostream>
using namespace std;

constexpr int minja = 0x3131;
constexpr int minmo = 0x314F;
constexpr int minhan = 0xAC00;

constexpr wchar_t key_sym[4][11] = {
	{L"1234567890"},
	{L"qwertyuiop"},
	{L"asdfghjkl"},
	{L"zxcvbnm"}
};

constexpr wchar_t shift_key_sym[4][11] = {
	{L"1234567890"},
	{L"QWERTYUIOP"},
	{L"ASDFGHJKL"},
	{L"ZXCVBNM"}
};

constexpr wchar_t hankey_sym[4][11] = {
	{L"1234567890"},
	{L"ㅂㅈㄷㄱㅅㅛㅕㅑㅐㅔ"},
	{L"ㅁㄴㅇㄹㅎㅗㅓㅏㅣ"},
	{L"ㅋㅌㅊㅍㅠㅜㅡ"}
};

constexpr wchar_t shift_hankey_sym[4][11] = {
	{L"1234567890"},
	{L"ㅃㅉㄸㄲㅆㅛㅕㅑㅒㅖ"},
	{L"ㅁㄴㅇㄹㅎㅗㅓㅏㅣ"},
	{L"ㅋㅌㅊㅍㅠㅜㅡ"}
};

int jatocho(int jaum)
{
	switch (jaum)
	{
	case 0:
		return 0;
	case 1:
		return 1;
	case 3:
		return 2;
	case 6:
		return 3;
	case 7:
		return 4;
	case 8:
		return 5;
	case 16:
		return 6;
	case 17:
		return 7;
	case 18:
		return 8;
	case 20:
		return 9;
	case 21:
		return 10;
	case 22:
		return 11;
	case 23:
		return 12;
	case 24:
		return 13;
	case 25:
		return 14;
	case 26:
		return 15;
	case 27:
		return 16;
	case 28:
		return 17;
	case 29:
		return 18;
	}
	return 0;
}

int jatojong(int jaum)
{
	if (jaum <= 6)
		return jaum + 1;
	else if (jaum <= 17)
		return jaum;
	else if (jaum <= 23)
		return jaum - 1;
	else if (jaum <= 29)
		return jaum - 2;

	return 0;
}

int jongtoja(int jaum)
{
	if (jaum <= 7)
		return jaum - 1;
	else if (jaum <= 17)
		return jaum;
	else if (jaum <= 22)
		return jaum + 1;
	else if (jaum <= 27)
		return jaum + 2;

	return 0;
}

wchar_t han_com(wchar_t top, wchar_t mid, wchar_t bottom)
{
	int topn = jatocho(((int)top - (int)minja));

	if (top == 0)
	{
		topn = 0;
	}
	int midn = (int)mid - (int)minmo;
	if (mid == 0)
	{
		midn = 0;
	}
	int botn = jatojong((int)bottom - (int)minja);
	if (bottom == 0)
	{
		botn = 0;
	}
	wchar_t result = (wchar_t) ((int)minhan + 588 * topn + 28 * midn + botn);
	return result;
}

bool isjaum(wchar_t top)
{
	wchar_t c = (wchar_t) ((int)top - (int)minja);

	if (0 <= c && c < 30)
		return true;
	else
		return false;
}

bool ismoum(wchar_t mid)
{
	wchar_t c = (wchar_t) ((int)mid - (int)minmo);
	if (0 <= c && c < 21)
		return true;
	else
		return false;
}

// 초성to종성
wchar_t chotojong(wchar_t top)
{
	wchar_t topn = (wchar_t) ((int)top - (int)minja);
	wchar_t botn = (wchar_t) 0x11A08;
	if (9 <= topn)
	{
		botn += 11;
	}
	else if (6 <= topn)
	{
		botn += 10;
	}
	else if (3 <= topn)
	{
		botn += 3;
	}
	else if (2 <= topn)
	{
		botn += 1;
	}

	return botn;
}

// 밭침이 없는 초성 중성만 있는 문자일 때.
bool is_chojung(wchar_t a)
{
	if ((int)a < (int)minhan)
		return false;
	int batchim = (((int)a - (int)minhan) % 588) % 28;
	if (batchim == 0)
		return true;
	else
		return false;
}

// 밭침이 있는 글자일때
bool is_batchim(wchar_t a)
{
	if ((int)a < (int)(int)minhan)
		return false;
	int batchim = (((int)a - (int)minhan) % 588) % 28;
	if (batchim != 0)
		return true;
	else
		return false;
}

// 이중받침글자일때
wchar_t is_batchim_pair(wchar_t a)
{
	int batchim = (((int)a - (int)minhan) % 588) % 28;
	switch (batchim)
	{
	case 3:
		return L'ㅅ';
	case 5:
		return L'ㅈ';
	case 6:
		return L'ㅎ';
	case 9:
		return L'ㄱ';
	case 10:
		return L'ㅁ';
	case 11:
		return L'ㅂ';
	case 12:
		return L'ㅅ';
	case 13:
		return L'ㅌ';
	case 14:
		return L'ㅍ';
	case 15:
		return L'ㅎ';
	case 18:
		return L'ㅅ';
	}

	return (wchar_t) 0;
}

int voca_word(wchar_t * arr, wchar_t n, int i)
{
	arr[i] = n;
	int len = wcslen(arr);
	for (int k = i + 1; k < len; ++k)
	{
		arr[k] = arr[k + 1];
	}
	arr[len - 1] = 0;
	return len - 1;
}

void hankey_arrange(wchar_t * arr)
{
	int len = wcslen(arr);
	for (int i = 0; i < len - 1; ++i)
	{
		wchar_t wo = arr[i];
		wchar_t wt = arr[i + 1];
		if (isjaum(wo) && ismoum(wt))
		{
			len = voca_word(arr, han_com(wo, wt, 0), i);
			continue;
		}
		else if (ismoum(wo) && ismoum(wt))
		{
			switch (wo)
			{
			case L'ㅗ':
				{
					switch (wt)
					{
					case L'ㅏ':
						len = voca_word(arr, L'ㅘ', i);
						break;
					case L'ㅐ':
						len = voca_word(arr, L'ㅙ', i);
						break;
					case L'ㅣ':
						len = voca_word(arr, L'ㅚ', i);
						break;
					default:
						break;
					}
				}
				break;
			case L'ㅜ':
				{
					switch (wt)
					{
					case L'ㅓ':
						len = voca_word(arr, L'ㅝ', i);
						break;
					case L'ㅔ':
						len = voca_word(arr, L'ㅞ', i);
						break;
					case L'ㅣ':
						len = voca_word(arr, L'ㅟ', i);
						break;
					default:
						break;
					}
				}
				break;
			case L'ㅡ':
				{
					if (wt == L'ㅣ')
					{
						len = voca_word(arr, L'ㅢ', i);
					}
				}
				break;
			default:
				break;
			}
		}
		else if (is_chojung(wo) && isjaum(wt))
		{
			int jj = jatojong((int)wt - minja);
			wchar_t ful = (wchar_t) ((int)wo + jj);
			len = voca_word(arr, ful, i);
		}
		else if (is_batchim(wo) && isjaum(wt))
		{
			int batchim = (((int)wo - minhan) % 588) % 28;
			if (batchim == 1 && wt == L'ㅅ')
			{
				len = voca_word(arr, (wchar_t) ((int)arr[i] + 2), i);
			}
			else if (batchim == 4 && wt == L'ㅈ')
			{
				len = voca_word(arr, (wchar_t) ((int)arr[i] + 1), i);
			}
			else if (batchim == 4 && wt == L'ㅎ')
			{
				len = voca_word(arr, (wchar_t) ((int)arr[i] + 2), i);
			}
			else if (batchim == 8)
			{
				switch (wt)
				{
				case L'ㄱ':
					len = voca_word(arr, (wchar_t) ((int)arr[i] + 1), i);
					break;
				case L'ㅁ':
					len = voca_word(arr, (wchar_t) ((int)arr[i] + 2), i);
					break;
				case L'ㅂ':
					len = voca_word(arr, (wchar_t) ((int)arr[i] + 3), i);
					break;
				case L'ㅅ':
					len = voca_word(arr, (wchar_t) ((int)arr[i] + 4), i);
					break;
				case L'ㅌ':
					len = voca_word(arr, (wchar_t) ((int)arr[i] + 5), i);
					break;
				case L'ㅍ':
					len = voca_word(arr, (wchar_t) ((int)arr[i] + 6), i);
					break;
				case L'ㅎ':
					len = voca_word(arr, (wchar_t) ((int)arr[i] + 7), i);
					break;
				}
			}
			else if (batchim == 17 && wt == L'ㅅ')
			{
				len = voca_word(arr, (wchar_t) ((int)arr[i] + 1), i);
			}
		}
		else if (is_chojung(wo) && ismoum(wt))
		{
			int midn = (((int)wo - (int)minhan) % 588) / 28;
			int aa=28;
			switch (midn)
			{
			case 8:
				{
					switch (wt)
					{
					case L'ㅏ':
						len = voca_word(arr, (wchar_t) ((int)arr[i] + 1*aa), i);
						break;
					case L'ㅐ':
						len = voca_word(arr, (wchar_t) ((int)arr[i] + 2*aa), i);
						break;
					case L'ㅣ':
						len = voca_word(arr, (wchar_t) ((int)arr[i] + 3*aa), i);
						break;
					default:
						break;
					}
				}
				break;
			case 13:
				{
					switch (wt)
					{
					case L'ㅓ':
						len = voca_word(arr, (wchar_t) ((int)arr[i] + 1*aa), i);
						break;
					case L'ㅔ':
						len = voca_word(arr, (wchar_t) ((int)arr[i] + 2*aa), i);
						break;
					case L'ㅣ':
						len = voca_word(arr, (wchar_t) ((int)arr[i] + 3*aa), i);
						break;
					default:
						break;
					}
				}
				break;
			case 18:
				{
					if (wt == L'ㅣ')
					{
						len = voca_word(arr, (wchar_t) ((int)arr[i] + 1*aa), i);
					}
				}
				break;
			default:
				break;
			}
		}
		else if ((is_batchim(wo) && is_batchim_pair(wo) == 0) && ismoum(wt))
		{
			int batchim = (((int)wo - (int)minhan) % 588) % 28;
			int jj = jongtoja(batchim);
			wchar_t top = (wchar_t) (minja + jj);
			arr[i] = (wchar_t) ((int)arr[i] - batchim);
			arr[i + 1] = han_com(top, wt, 0);
		}
		else if (is_batchim_pair(wo) != 0 && ismoum(wt))
		{
			int batchim = (((int)wo - (int)minhan) % 588) % 28;
			switch (batchim)
			{
			case 3:
				arr[i] = (wchar_t) ((int)arr[i] - 2);
				break;
			case 5:
				arr[i] = (wchar_t) ((int)arr[i] - 1);
				break;
			case 6:
				arr[i] = (wchar_t) ((int)arr[i] - 2);
				break;
			case 9:
				arr[i] = (wchar_t) ((int)arr[i] - 1);
				break;
			case 10:
				arr[i] = (wchar_t) ((int)arr[i] - 2);
				break;
			case 11:
				arr[i] = (wchar_t) ((int)arr[i] - 3);
				break;
			case 12:
				arr[i] = (wchar_t) ((int)arr[i] - 4);
				break;
			case 13:
				arr[i] = (wchar_t) ((int)arr[i] - 5);
				break;
			case 14:
				arr[i] = (wchar_t) ((int)arr[i] - 6);
				break;
			case 15:
				arr[i] = (wchar_t) ((int)arr[i] - 7);
				break;
			case 18:
				arr[i] = (wchar_t) ((int)arr[i] - 1);
				break;
			}
			arr[i + 1] = han_com(is_batchim_pair(wo), wt, 0);
		}
	}
}
