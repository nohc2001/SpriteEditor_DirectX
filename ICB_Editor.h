#ifndef ICB_EDITOR_H
#define ICB_EDITOR_H

#include "InsideCodeBake.h"
#include "Utill_SpaceMath.h"

struct ICBE_ToolData {
	char* name;
	unsigned int len;
	ICB_Context* ecs;
};

class ICB_Editor {
public:
	ICB_Editor() {
	}
	~ICB_Editor() {
	}

	bool visible = true;
	bool focus = false;
	shp::rect4f loc;

	InsideCode_Bake* SelectedICB;

	fmvecarr<fmlwstr> codeLines;
	fmvecarr<unsigned int> codeLine_expend; // v & 1 => isexpend / v>>1 => codelinenum
	fmlwstr currentErrorMsg;
	unsigned int selected_codeline;
	unsigned int selected_charindex;
	float codeline_height = 20.0f;
	float linemargin = 1.2f;
	float linenumwid = 1.5f;
	int Xofffset = 0;
	//float cursorAlpha = 1.0f;
	float headerRate = 2.0f;
	float CodeEditorRate = 0.75f;
	float SliderWidRate = 0;
	float SliderRate = 0; // to 1.0f
	float stackT = 0;
	bool BeShift = false;
	bool CapsLock = false;
	bool isMoving = false;
	bool isExpend = false;
	float tipmov = 5.0f;
	shp::rect4f movOffset;
	bool showVarmod = true;
	bool inCtrl = false;

	//fold propertys
	shp::vec2f object_position;
	bool befold = false;
	fmvecarr<ICBE_ToolData> tools;

	static constexpr float lifeconst[10] = { 2.0f, 3.0f, 5.0f, 7.0f, 11.0f, 13.0f, 17.0f, 19.0f, 23.0f, 29.0f };

	float life[10] = {};

	int blockstack = 0;

	void CodeLinesClear() {
		for (int i = 0; i < codeLines.size(); ++i) {
			codeLines[i].release();
			codeLines[i].NULLState();
		}
		codeLines.up = 0;
	}

	void PushNewCodeLine() {
		codeLines.push_back(fmlwstr());
		codeLines.last().NULLState();
		codeLines.last().Init(8, false);
	}

	void AddExpend(unsigned int index) {
		int low = 0;
		int high = codeLine_expend.size() - 1;
		while (low <= high) {
			int mid = low + (high - low) / 2;
			int presindex = codeLine_expend[mid] >> 1;
			if (presindex == index)
			{
				low = mid;
				return;
				break;
			}
			else if (presindex > index)
				high = mid - 1;
			else
				low = mid + 1;
		}

		codeLine_expend.insert(low, (index << 1) + 1);
	}

	unsigned int GetExpend(unsigned int index) {
		int low = 0;
		int high = codeLine_expend.size() - 1;
		while (low <= high) {
			int mid = low + (high - low) / 2;
			int presindex = codeLine_expend[mid] >> 1;
			if (presindex == index)
			{
				low = mid;
				return codeLine_expend[low] & 1;
			}
			else if (presindex > index)
				high = mid - 1;
			else
				low = mid + 1;
		}

		return 0;
	}

	void SetExpend(unsigned int index, bool expend) {
		int low = 0;
		int high = codeLine_expend.size() - 1;
		while (low <= high) {
			int mid = low + (high - low) / 2;
			int presindex = codeLine_expend[mid] >> 1;
			if (presindex == index)
			{
				low = mid;
				int v = 0;
				if (expend) v = 1;
				codeLine_expend[low] = ((codeLine_expend[low] >> 1) << 1) + v;
				break;
			}
			else if (presindex > index)
				high = mid - 1;
			else
				low = mid + 1;
		}
	}

	shp::vec2i GetExpendLimit(unsigned int index) {
		bool enable = false;
		shp::vec2i pair = shp::vec2i(-1, -1);
		unsigned int n = index;
		int si = 0;
		for (int i = 0; i < codeLines[index].size(); ++i) {
			if (codeLines[index][i] == L'{') {
				enable = true;
				si = i + 1;
				if (si >= codeLines[index].size()) {
					si = 0;
					n += 1;
				}
				break;
			}
		}
		if (!enable) return pair;

		int stack = 1;
		bool end = false;
		for (int i = n; i < codeLines.size(); ++i) {
			for (int k = si; k < codeLines[i].size(); ++k) {
				if (codeLines[i][k] == L'{') {
					stack += 1;
				}
				else if (codeLines[i][k] == L'}') {
					stack -= 1;
					if (stack == 0) {
						end = true;
						pair.x = i;
						pair.y = k;
						return pair;
					}
				}
			}
			si = 0;
		}

		return pair;
	}

	shp::vec2i GetExpendStart(unsigned int endindex) {
		bool enable = false;
		shp::vec2i pair = shp::vec2i(-1, -1);
		unsigned int n = endindex;
		int si = 0;
		for (int i = 0; i < codeLines[endindex].size(); ++i) {
			if (codeLines[endindex][i] == L'}') {
				enable = true;
				si = i - 1;
				if (si < 0) {
					if (endindex != 0) {
						si = codeLines[endindex - 1].size() - 1;
						n -= 1;
					}
					else {
						si = 0;
						n = 0;
					}
				}
				break;
			}
		}
		if (!enable) return pair;

		int stack = 1;
		bool end = false;
		for (int i = n; i >= 0; --i) {
			for (int k = si; k >= 0; --k) {
				if (codeLines[i][k] == L'}') {
					stack += 1;
				}
				else if (codeLines[i][k] == L'{') {
					stack -= 1;
					if (stack == 0) {
						end = true;
						pair.x = i;
						pair.y = k;
						return pair;
					}
				}
			}

			if (i != 0) {
				si = codeLines[i - 1].size() - 1;
			}
			else si = 0;
		}

		return pair;
	}

	void WhenLineInsert(unsigned int index) {
		int low = 0;
		int high = codeLine_expend.size() - 1;
		while (low <= high) {
			int mid = low + (high - low) / 2;
			int presindex = codeLine_expend[mid] >> 1;
			if (presindex == index)
			{
				low = mid;
				//codeLine_expend.insert(low, (index<<1) + 1);
				//low += 1;
				break;
			}
			else if (presindex > index)
				high = mid - 1;
			else
				low = mid + 1;
		}

		for (int i = low; i < codeLine_expend.size(); ++i) {
			unsigned int v = codeLine_expend[i] & 1;
			codeLine_expend[i] = (((codeLine_expend[i] >> 1) + 1) << 1) + v;
		}
	}

	void WhenLineErase(unsigned int index) {
		int low = 0;
		int high = codeLine_expend.size() - 1;
		while (low <= high) {
			int mid = low + (high - low) / 2;
			int presindex = codeLine_expend[mid] >> 1;
			if (presindex == index)
			{
				low = mid;
				codeLine_expend.erase(low);
				break;
			}
			else if (presindex > index)
				high = mid - 1;
			else
				low = mid + 1;
		}

		for (int i = low; i < codeLine_expend.size(); ++i) {
			unsigned int v = codeLine_expend[i] & 1;
			codeLine_expend[i] = (((codeLine_expend[i] >> 1) - 1) << 1) + v;
		}
	}

	void WhenCharInsert(unsigned int index, unsigned int cindex, wchar_t wc) {
		if (wc == L'{') {
			AddExpend(index);
		}
	}

	void BeforeCharErase(unsigned int index, unsigned int cindex) {
		if (codeLines[index][cindex] == L'{') {
			int low = 0;
			int high = codeLine_expend.size() - 1;
			while (low <= high) {
				int mid = low + (high - low) / 2;
				int presindex = codeLine_expend[mid] >> 1;
				if (presindex == index)
				{
					low = mid;
					codeLine_expend.erase(low);
					return;
				}
				else if (presindex > index)
					high = mid - 1;
				else
					low = mid + 1;
			}
		}
	}

	void ImportBlockToCodeLine(code_sen* cs) {
		PushNewCodeLine();
		for (int k = 0; k < blockstack; ++k) {
			codeLines.last().push_back(L' ');
			codeLines.last().push_back(L' ');
		}
		codeLines.last().push_back(L'{');
		codeLines.last().c_str();
		AddExpend(codeLines.up - 1);

		blockstack += 1;

		for (int i = 0; i < cs->codeblocks->size(); ++i) {
			code_sen* scs = reinterpret_cast<code_sen*>(cs->codeblocks->at(i));
			if (scs->ck != codeKind::ck_blocks) {
				PushNewCodeLine();
				//codeLine_expend.push_back(false);

				for (int k = 0; k < blockstack; ++k) {
					codeLines.last().push_back(L' ');
					codeLines.last().push_back(L' ');
				}

				for (int k = 0; k < scs->maxlen; ++k) {
					char* word = scs->sen[k];
					int wlen = strlen(word);
					for (int u = 0; u < wlen; ++u) {
						codeLines.last().push_back((wchar_t)word[u]);
					}
					codeLines.last().push_back(L' ');
				}

				if (scs->ck != codeKind::ck_addFunction && (scs->ck != codeKind::ck_while && scs->ck != codeKind::ck_if)) {
					codeLines.last().push_back(L';');
				}

				codeLines.last().c_str();
			}
			else {
				ImportBlockToCodeLine(scs);
			}
		}

		blockstack -= 1;

		PushNewCodeLine();
		for (int k = 0; k < blockstack; ++k) {
			codeLines.last().push_back(L' ');
			codeLines.last().push_back(L' ');
		}
		codeLines.last().push_back(L'}');
		codeLines.last().c_str();
	}

	void ReadCodelines_FromFile(const char* filename) {
		CodeLinesClear();

		FILE* fp;
		fopen_s(&fp, filename, "rt");
		if (fp)
		{
			//fmlcstr* codetxt = (fmlcstr*)fm->_New(sizeof(fmlcstr), true);
			//codetxt->NULLState();
			//codetxt->Init(10, false);

			int max = 0;
			fseek(fp, 0, SEEK_END);
			max = ftell(fp);
			fclose(fp);

			int stack = 0;
			fopen_s(&fp, filename, "rt");
			int k = 0;
			PushNewCodeLine();
			while (k < max)
			{
				wchar_t c;
				char cc = (char)fgetc(fp);
				if (cc == EOF) {
					break;
				}
				c = cc;

				if (c == L'\n') {
					codeLines.last().c_str();
					PushNewCodeLine();
					continue;
				}
				else if (c == L'{') {
					AddExpend(codeLines.up - 1);
				}
				else if (cc == '\t') {
					codeLines.last().push_back(L' ');
					codeLines.last().push_back(L' ');
					k++;
					continue;
				}

				codeLines.last().push_back(c);
				k++;
			}
		}

		codeLines.last().c_str();
	}

	void Init(shp::rect4f _loc, InsideCode_Bake* sicb);

	void Render();

	void Event(DX_Event evt);
	
	void Update(float delta) {
		stackT += delta;
		//cursorAlpha = fabsf(sinf(2.5f*stackT));
		for (int i = 0; i < 10; ++i) {
			//float f = stackT / lifeconst[i];
			//life[i] = AnimClass::EaseOut(f - floorf(f), 3);
			life[i] = (1.0f + sinf(lifeconst[i] / 3.0f * stackT)) / 2.0f;
		}
	}
};
#endif