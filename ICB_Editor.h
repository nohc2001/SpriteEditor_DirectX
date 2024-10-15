#ifndef ICB_EDITOR_H
#define ICB_EDITOR_H

#include "InsideCodeBake.h"
#include "Utill_SpaceMath.h"
#include "Sprite.h"
#include "DX11_UI.h"

struct ICBE_ToolData {
	char* name;
	unsigned int len;
	ICB_Context* ecs;
};

Sprite* PlayBtnSpr = nullptr;
Sprite* PauseBtnSpr = nullptr;
Sprite* StopBtnSpr = nullptr;
Sprite* StepBtnSpr = nullptr;
Sprite* StepInBtnSpr = nullptr;
Sprite* PresentExecutionSpr = nullptr;

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
	int SelectedCXT;

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

	// debug breakpoints
	fmvecarr<ui32> CodeLineBreakPoints;
	fmvecarr<ui32> funcjmp_stack; // for dbg step in

	// debug variable table data
	fmvecarr<byte8*> expend_variabletableaddress;

	static constexpr float lifeconst[10] = { 2.0f, 3.0f, 5.0f, 7.0f, 11.0f, 13.0f, 17.0f, 19.0f, 23.0f, 29.0f };

	float life[10] = {};

	int blockstack = 0;

	static void StaticInit();

	bool isVariableTableShowExpending(byte8* ptr) {
		for (int i = 0; i < expend_variabletableaddress.size(); ++i) {
			if (expend_variabletableaddress.at(i) == ptr) {
				return true;
			}
		}
		return false;
	}

	void ExpendVariableTable(byte8* variable_ptr) {
		if (isVariableTableShowExpending(variable_ptr) == false) {
			expend_variabletableaddress.push_back(variable_ptr);
		}
	}

	void ShrinkingVariableTable(byte8* variable_ptr) {
		for (int i = 0; i < expend_variabletableaddress.size(); ++i) {
			if (expend_variabletableaddress.at(i) == variable_ptr) {
				expend_variabletableaddress.erase(i);
			}
		}
	}

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

	shp::vec2f RenderVariableState(NamingData nd, shp::vec2f startPos, shp::rect4f limitloc, byte8* sps, int arrayindex) {
		if (shp::bPointInRectRange(startPos, limitloc)) {
			return startPos;
		}
		
		constexpr float ymargin = 20;
		constexpr float fontsiz = 4;
		const float xmargin = (limitloc.lx - startPos.x) / 3;
		float stackY = startPos.y;
		wchar_t typestr[64] = {};
		wchar_t namestr[256] = {};
		wchar_t indexstr[16] = {};
		int i = 0;
		int tmax = min(nd.td->name.size(), 64);
		int nmax = 0;
		for (; i < tmax; ++i) {
			typestr[i] = nd.td->name.at(i);
		}
		typestr[i] = 0;
		
		if (arrayindex >= 0) {
			_itow_s(arrayindex, indexstr, 10);
			namestr[0] = L'[';
			int len = wcslen(indexstr);
			for (int i = 0; i < len; ++i) {
				namestr[i + 1] = indexstr[i];
			}
			namestr[len + 1] = L']';
			namestr[len + 2] = 0;
			nmax = len + 1;
		}
		else {
			i = 0;
			nmax = min((strlen(nd.name)), 255);
			for (; i < nmax; ++i) {
				namestr[i] = nd.name[i];
			}
			namestr[nmax] = 0;
		}
		
		draw_string(typestr, tmax, fontsiz, shp::rect4f(startPos.x, startPos.y - ymargin, startPos.x + xmargin, startPos.y), DX11Color(0.0f, 0.0f, 0.0f, 1.0f), 0.1f);
		draw_string(namestr, nmax, fontsiz, shp::rect4f(startPos.x+xmargin, startPos.y - ymargin, startPos.x + 2*xmargin, startPos.y), DX11Color(0.0f, 0.0f, 0.0f, 1.0f), 0.1f);
		
		if (nd.td->typetype == 'b') {
			wchar_t valuestr[128] = {};
			if (nd.td->name.operator==("int")) {
				int value = *(int*)sps;
				_itow_s(value, valuestr, 10);
			}
			else if (nd.td->name.operator==("uint")) {
				ui32 value = *(ui32*)sps;
				_ui64tow_s(value, valuestr, 32, 10);
			}
			else if (nd.td->name.operator==("short")) {
				short value = *(short*)sps;
				_itow_s(value, valuestr, 10);
			}
			else if (nd.td->name.operator==("ushort")) {
				unsigned short value = *(unsigned short*)sps;
				_itow_s(value, valuestr, 10);
			}
			else if (nd.td->name.operator==("char")) {
				char value = *(char*)sps;
				valuestr[0] = value;
				valuestr[1] = L'(';
				wchar_t nstr[16] = {};
				_itow_s((int)value, nstr, 10);
				int l = wcslen(nstr);
				int i = 0;
				for (; i < l; ++i) {
					valuestr[i + 2] = nstr[i];
				}
				valuestr[i] = L')';
				valuestr[i + 1] = 0;
			}
			else if (nd.td->name.operator==("uchar")) {
				unsigned char value = *(unsigned char*)sps;
				_itow_s(value, valuestr, 10);
			}
			else if (nd.td->name.operator==("float")) {
				float value = *(float*)sps;
				string a = to_string(value);
				int i = 0;
				for (; i < a.size(); ++i) {
					valuestr[i] = a[i];
				}
				valuestr[i] = 0;
			}
			else if (nd.td->name.operator==("bool")) {
				bool value = *(bool*)sps;
				if (value) {
					wcscpy_s(valuestr, L"true");
				}
				else {
					wcscpy_s(valuestr, L"false");
				}
			}

			int len = wcslen(valuestr);
			draw_string(valuestr, len, fontsiz, shp::rect4f(startPos.x + 2 * xmargin, startPos.y - ymargin, limitloc.lx, startPos.y), DX11Color(0.0f, 0.0f, 0.0f, 1.0f), 0.1f);
		}
		else if (nd.td->typetype == 's') {
			//struct
			struct_data* sd = (struct_data*)nd.td->structptr;
			if (isVariableTableShowExpending(sps)) {
				//expending
				sps -= nd.td->typesiz;
				stackY -= ymargin;
				for (int i = 0; i < sd->member_data.size(); i++) {
					startPos = RenderVariableState(sd->member_data.at(i), shp::vec2f(startPos.x, stackY), limitloc, sps, -1);
					stackY = startPos.y;
					stackY -= ymargin;
					sps += sd->member_data.at(i).td->typesiz;
				}
				stackY += ymargin;
			}
			//else {
				//shrinking
			//}
		}
		else if (nd.td->typetype == 'p') {
			//ptr
			ui64 value = reinterpret_cast<ui64>(sps);
			wchar_t valuestr[32] = {};
			_ui64tow_s(value, valuestr, 32, 16);
			int len = wcslen(valuestr);
			draw_string(valuestr, len, fontsiz, shp::rect4f(startPos.x + 2 * xmargin, startPos.y - ymargin, limitloc.lx, startPos.y), DX11Color(0.0f, 0.0f, 0.0f, 1.0f), 0.1f);
		}
		else if (nd.td->typetype == 'a') {
			//array
			int capacity = nd.td->typesiz / ((type_data*)nd.td->structptr)->typesiz;
			type_data* std = (type_data*)nd.td->structptr;
			stackY -= ymargin;
			sps -= nd.td->typesiz;
			if (isVariableTableShowExpending(sps)) {
				//expending
				for (int i = 0; i < capacity; ++i) {
					NamingData nd;
					nd.td = std;
					nd.add_address = 0;
					nd.name = nullptr;
					stackY = RenderVariableState(nd, shp::vec2f(startPos.x, stackY), limitloc, sps, -1).y;
					stackY -= ymargin;
					sps += std->typesiz;
				}
				stackY += ymargin;
			}
			//else {
			//	//shrinking
			//}
		}
		return shp::vec2f(startPos.x, stackY);
	}

	shp::vec2f EventVariableState(NamingData nd, shp::vec2f startPos, shp::rect4f limitloc, byte8* sps, int arrayindex, shp::vec2f mpos) {
		if (shp::bPointInRectRange(startPos, limitloc)) {
			return startPos;
		}

		constexpr float ymargin = 20;
		constexpr float fontsiz = 3;
		const float xmargin = (limitloc.lx - startPos.x) / 3;
		float stackY = startPos.y;
		int i = 0;

		shp::rect4f cloc = shp::rect4f(startPos.x, startPos.y - ymargin, limitloc.lx, startPos.y);
		if (shp::bPointInRectRange(mpos, cloc)) {
			if (isVariableTableShowExpending(sps)) {
				ShrinkingVariableTable(sps);
			}
			else {
				ExpendVariableTable(sps);
			}
		}
		if (isVariableTableShowExpending(sps))
		{
			if (nd.td->typetype == 's') {
				//struct
				struct_data* sd = (struct_data*)nd.td->structptr;

				//expending
				sps -= nd.td->typesiz;
				stackY -= ymargin;
				for (int i = 0; i < sd->member_data.size(); i++) {
					stackY = EventVariableState(sd->member_data.at(i), shp::vec2f(startPos.x, stackY), limitloc, sps, -1, mpos).y;
					stackY -= ymargin;
					sps += sd->member_data.at(i).td->typesiz;
				}
				stackY += ymargin;
			}
			else if (nd.td->typetype == 'a') {
				//array
				int capacity = nd.td->typesiz / ((type_data*)nd.td->structptr)->typesiz;
				type_data* std = (type_data*)nd.td->structptr;
				stackY -= ymargin;
				sps -= nd.td->typesiz;
				//expending
				for (int i = 0; i < capacity; ++i) {
					NamingData nd;
					nd.td = std;
					nd.add_address = 0;
					nd.name = nullptr;
					stackY = EventVariableState(nd, shp::vec2f(startPos.x, stackY), limitloc, sps, -1, mpos).y;
					stackY -= ymargin;
					sps += std->typesiz;
				}
				stackY += ymargin;
			}
		}
		return shp::vec2f(startPos.x, stackY);
	}

	//not using function (init with icb)
	void Init(shp::rect4f _loc, InsideCode_Bake* sicb);
	//with filename
	void Init(shp::rect4f _loc, const char* filename);
	//with codelines
	void Init(shp::rect4f _loc, fmvecarr<fmlwstr>& cLines);
	//with text data
	void Init(shp::rect4f _loc, char* code_str);

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