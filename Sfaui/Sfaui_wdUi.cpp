#include "Sfaui_wdUi.h"
#include <sstream>



using namespace nonstd;
static bool show_menu_panel = false;
static bool openset = false;
static bool openmap = false;
// 修复1：把弹窗显示标记提升到静态全局，每帧不重置
static bool showSavePathWin = false;
static bool showWriteCodeWin = false; // 独立代码弹窗开关

void Windows_Ui()
{
	SfauiSettingConfig& cfg = g_SettingConfig;

	if (openmap)
	{
		ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2(SFAUI_SW, SFAUI_SH));
		ImGui::Begin("地图查看器 / Map Viewer", nullptr, ImGuiWindowFlags_NoTitleBar);
		DrawMapCanvasOnlyDrag();
	
		ImGui::End();
	}

	ImGui::SetNextWindowPos(ImVec2((SFAUI_SW - IMAGESIZE.x * 4.1f) * 0.9f, SFAUI_SH * 0.01f), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(-1, -1));
	ImGuiWindowFlags win_flags = 0;
	win_flags |= ImGuiWindowFlags_NoTitleBar;
	win_flags |= ImGuiWindowFlags_NoMove;
	win_flags |= ImGuiWindowFlags_NoBackground;
	win_flags |= ImGuiWindowFlags_NoDecoration;

	// 修复2：click临时变量放在外层，不要嵌套在Begin内部重置
	bool clickOpenSet = false;
	bool clickSavePath = false;
	bool clickWrietWin = false;

	if (ImGui::Begin("Window", nullptr, win_flags))
	{
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(200, 50, 100, 100));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));

		if (ImGui::ImageButton("ic_undo", g_tex_pool["ic_undo"], IMAGESIZE, ImVec2(0, 0), ImVec2(1, 1)))
		{
			g_HistoryMgr.Undo();
			Sfaui_log("redo:%zu,undo:%zu\n", g_HistoryMgr.CanRedo(), g_HistoryMgr.CanUndo());
			ifundo = true;
		}
		ImGui::SameLine();
		if (ImGui::ImageButton("ic_redo", g_tex_pool["ic_dedo"], IMAGESIZE, ImVec2(0, 0), ImVec2(1, 1)))
		{
			Sfaui_log("redo:%zu,undo:%zu\n", g_HistoryMgr.CanRedo(), g_HistoryMgr.CanUndo());
			g_HistoryMgr.Redo();
			ifredo = true;
		}
		ImGui::SameLine();
		if (ImGui::ImageButton("menu_white", g_tex_pool["menu_white"], IMAGESIZE, ImVec2(0, 0), ImVec2(1, 1)))
		{
			show_menu_panel = !show_menu_panel;
		}
		if (show_menu_panel)
		{
			ImGui::SetNextWindowPos(ImVec2(SFAUI_SW - 270, 80));
			ImGui::Begin("菜单面板 menu", &show_menu_panel);

			ImGui::Spacing();
			if (ImGui::Button("设置 set", ImVec2(-1, 50)))
			{
				openset = true;
				clickOpenSet = true;
			}

			if (ImGui::Button("层级关系 flood map", ImVec2(-1, 50)))
			{
				if (LVBuild.size() == WasChose.size() &&
					Visibility.size() == WasChose.size() &&
					WasChose.size() == All_mRNA.size())
				{
					openmap = true;
				}
			}

			if (ImGui::Button("保存项目 save project", ImVec2(-1, 50)))
			{
				clickSavePath = true;
			}

			if (ImGui::Button("生成代码 make code", ImVec2(-1, 50)))
			{
				clickWrietWin = true;
			}
			ImGui::End();
		}
		ImGui::PopStyleColor(3);
	}
	ImGui::End();

	// 设置弹窗
	if (clickOpenSet)
		ImGui::OpenPopup("编辑器设置 / Editor Settings");
	DrawSettingPanel(openset);

	// ========== 保存项目弹窗（独立开关 showSavePathWin） ==========
	if (clickSavePath)
	{
		ImGui::OpenPopup("保存项目路径");
		showSavePathWin = true; // 同步打开标记
	}
	if (ImGui::BeginPopupModal("保存项目路径", &showSavePathWin, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("输入保存文件完整路径 file path：");
		ImGui::InputText("##path", Sfaui_SavePathBuf, IM_ARRAYSIZE(Sfaui_SavePathBuf));
		ImGui::Spacing();

		if (ImGui::Button("确认保存 save", ImVec2(120, 0)))
		{
			SaveProjectBinary(Sfaui_SavePathBuf);
			showSavePathWin = false;
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("取消 cencl", ImVec2(120, 0)))
		{
			showSavePathWin = false;
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	// ========== 生成代码弹窗（全新独立静态开关 showWriteCodeWin） ==========
	if (clickWrietWin)
	{
		ImGui::OpenPopup("生成代码路径");
		showWriteCodeWin = true;
	}
	if (ImGui::BeginPopupModal("生成代码路径", &showWriteCodeWin, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("输入生成文件完整路径 file path：");
		ImGui::InputText("##path", Sfaui_WriteCodePathBuf, IM_ARRAYSIZE(Sfaui_WriteCodePathBuf));
		ImGui::Spacing();

		if (ImGui::Button("确认保存 save", ImVec2(120, 0)))
		{
			std::string code = MakeCode();
			FILE* fp = fopen(Sfaui_WriteCodePathBuf, "w");
			if (fp != nullptr)
			{
				fputs(code.c_str(), fp);
				fclose(fp);
			}
			showWriteCodeWin = false;
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("取消 cencl", ImVec2(120, 0)))
		{
			showWriteCodeWin = false;
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}

void DrawAdaptiveTextFrame(ImDrawList* draw, ImVec2 pos, const char* text, float minBoxW, float boxH, ImU32 col)
{
	float fontSize = ImGui::GetFontSize();
	ImFont* font = ImGui::GetFont();
	float pad = 8.0f;

	ImVec2 textSize = font->CalcTextSizeA(fontSize, FLT_MAX, 0.0f, text);
	float boxW = (textSize.x + pad * 2 > minBoxW) ? (textSize.x + pad * 2) : minBoxW;

	ImVec2 rectMin = pos;
	ImVec2 rectMax = ImVec2(pos.x + boxW, pos.y + boxH);

	draw->AddRect(rectMin, rectMax, col, 3.0f, 0, 1.5f);

	ImVec2 textPos;
	textPos.x = rectMin.x + (boxW - textSize.x) * 0.5f;
	textPos.y = rectMin.y + (boxH - textSize.y) * 0.5f;

	draw->AddText(font, fontSize, textPos, col, text);
}


void DrawMapCanvasOnlyDrag()
{
	static ImVec2 mapOffset = ImVec2(0.0f, 0.0f);
	static bool isDragging = false;
	static ImVec2 dragStartMouse;
	static ImVec2 dragStartOffset;

	ImVec2 availSize = ImGui::GetContentRegionAvail();
	//ImGui::SetCursorPos(ImVec2(0, 10));
	if (ImGui::Button("重置画布位置 / Reset Offset", ImVec2(160, 32)))
	{
		mapOffset = ImVec2(0.0f, 0.0f);
	}
	ImGui::SameLine();
	if (ImGui::Button("退出 / back", ImVec2(160, 32)))
	{
		openmap = false;
	}
	ImGui::BeginChild("MapDragCanvas", availSize, true,
		ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

	ImDrawList* draw = ImGui::GetWindowDrawList();
	ImVec2 canvasOrigin = ImGui::GetCursorScreenPos();
	ImGuiIO& io = ImGui::GetIO();
	bool hovered = ImGui::IsWindowHovered();

	ImU32 gridCol = IM_COL32(70, 80, 95, 110);
	float cellSize = 50.0f;
	int gridRange = 50;

	for (int x = -gridRange; x <= gridRange; x++)
	{
		float xPos = canvasOrigin.x + mapOffset.x + x * cellSize;
		draw->AddLine(
			ImVec2(xPos, canvasOrigin.y + mapOffset.y - gridRange * cellSize),
			ImVec2(xPos, canvasOrigin.y + mapOffset.y + gridRange * cellSize),
			gridCol, 1.0f
		);
	}
	for (int y = -gridRange; y <= gridRange; y++)
	{
		float yPos = canvasOrigin.y + mapOffset.y + y * cellSize;
		draw->AddLine(
			ImVec2(canvasOrigin.x + mapOffset.x - gridRange * cellSize, yPos),
			ImVec2(canvasOrigin.x + mapOffset.x + gridRange * cellSize, yPos),
			gridCol, 1.0f
		);
	}

	//中心十字
	draw->AddLine(
		ImVec2(canvasOrigin.x + mapOffset.x - 400, canvasOrigin.y + mapOffset.y),
		ImVec2(canvasOrigin.x + mapOffset.x + 400, canvasOrigin.y + mapOffset.y),
		IM_COL32(255, 90, 90, 220), 2.2f
	);
	draw->AddLine(
		ImVec2(canvasOrigin.x + mapOffset.x, canvasOrigin.y + mapOffset.y - 400),
		ImVec2(canvasOrigin.x + mapOffset.x, canvasOrigin.y + mapOffset.y + 400),
		IM_COL32(255, 90, 90, 220), 2.2f
	);

	
	std::vector<std::pair<float, float>> levelStartY;
	std::vector<std::pair<float, float>> levelStartX;
	float suojin = 0;
	if (LVBuild.size() <= 0)
	{
		ImVec2 framePos(canvasOrigin.x + mapOffset.x + 150, canvasOrigin.y + mapOffset.y + 150);
		DrawAdaptiveTextFrame(draw, framePos, "empty", 120.0f, 26.0f, IM_COL32(255, 255, 255, 255));
	}


	std::pair<float, float> levelStartY_help = { -1,-1 };
	std::pair<float, float> levelStartX_help = { -1,-1 };

	std::vector<float>_levepair_;
	std::vector<float>_levepairy_;
	for (int i = 0; i < LVBuild.size(); i++)
	{
		auto& arr = All_mRNA[i];
		if (LVBuild[i] != 0)
		{
			/*
			if(levelStartY_help.first==-1){
			 levelStartY_help.first=  i * 33 + canvasOrigin.y + mapOffset.y + 200;
			 levelStartY_help.first= suojin;
			 }
			if(levelStartY_help.first!=-1&&levelStartY_help.second==-1){
			 levelStartY_help.second=  i * 33 + canvasOrigin.y + mapOffset.y + 200;
			 levelStartY_help.second= suojin;
			 }
			if(levelStartY_help.first!=-1&&levelStartY_help.second!=-1){
				levelStartY.push_back(levelStartY_help);
				levelStartX.push_back(levelStartY_help);
				levelStartY_help={-1,-1};
				levelStartX_help={-1,-1};
				}*/
			suojin = LVBuild[i] * 45;
		}

		ImVec2 framePos2(
			suojin + canvasOrigin.x + mapOffset.x + 150,
			i * 33 + canvasOrigin.y + mapOffset.y + 200
		);
		draw->AddLine(
			framePos2,
			ImVec2(framePos2.x-20, framePos2.y),
			IM_COL32(120, 180, 255, 180),
			1.5f
		);
		_levepair_.push_back(framePos2.x - 20);
		_levepairy_.push_back(framePos2.y);
		DrawAdaptiveTextFrame(draw, framePos2, any_cast<std::string>(arr[REALID]).c_str(), 120.0f, 26.0f, IM_COL32(255, 255, 255, 255));

	}
	
	// 第二遍正式绘制
	suojin = 0;
	for(int i = 0; i < _levepair_.size(); i++)
	{
		float fl = _levepair_[i];
		float fly = _levepairy_[i];
		for (int j = i+1; j < _levepair_.size(); j++)
		{
			if ((int)fl == (int)_levepair_[j]) {
				draw->AddLine(
					ImVec2(fl, fly),
					ImVec2(_levepair_[j], _levepairy_[j]),
					IM_COL32(120, 180, 255, 180),
					1.5f
				);
			}
		}	
	}
	


	//拖拽逻辑
	if (hovered && io.MouseDown[0])
	{
		if (!isDragging)
		{
			isDragging = true;
			dragStartMouse = io.MousePos;
			dragStartOffset = mapOffset;
		}
		ImVec2 dragDelta = ImVec2(
			io.MousePos.x - dragStartMouse.x,
			io.MousePos.y - dragStartMouse.y
		);
		mapOffset.x = dragStartOffset.x + dragDelta.x;
		mapOffset.y = dragStartOffset.y + dragDelta.y;
	}
	else
	{
		isDragging = false;
	}

	ImGui::EndChild();

	ImGui::Spacing();



}


void DrawSettingPanel(bool& openSetPopup)
{
	if (ImGui::BeginPopupModal("编辑器设置 / Editor Settings", &openSetPopup,
		ImGuiWindowFlags_NoScrollbar
		| ImGuiWindowFlags_NoDecoration
		| ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_AlwaysAutoResize))
	{
		// 全屏弹窗尺寸铺满屏幕
		ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize, ImGuiCond_Always);

		// 全局半透明 0~1，越小越透明
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.85f);
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(6, 6));

		SfauiSettingConfig& cfg = g_SettingConfig;

		if (ImGui::CollapsingHeader("基础设置 / Basic Settings"))
		{
			ImGui::Checkbox("滑动屏幕设置坐标 / Use Touch To Adjust ImVec2", &cfg.UseScreen_SetImvec2);
			ImGui::Checkbox("显示日志窗口 / Show Log Window", &cfg.showLogWindow);
			//ImGui::Checkbox("显示辅助网格 / Show Auxiliary Grid", &cfg.showGrid);
			//ImGui::Checkbox("开启网格吸附 / Snap To Grid Lines", &cfg.snapGrid);
			//ImGui::DragFloat("网格尺寸 / Grid Size", &cfg.gridSize, 0.5f, 2.0f, 50.0f);

			ImGui::Spacing();
			ImGui::DragInt("撤销最大步数 / Max Undo History Count", &cfg.undoMaxCount, 1, 10, 200);
		}

		if (ImGui::CollapsingHeader("样式主题 / Style & Theme"))
		{
			ImGui::Checkbox("编辑器跟随项目样式 / Editor Follow Project Style", &cfg.followProjectStyle);
			ImGui::Checkbox("绘制UI跟随项目样式 / Render UI Follow Project Style", &cfg.drawuifollowProjectStyle);
			ImGui::DragFloat("全局界面透明度 / Global UI Opacity", &cfg.alphaGlobal, 0.01f, 0.2f, 1.0f);
			ImGui::Spacing();

			ImGui::Text("圆角参数 / Radius Settings");
			ImGui::DragFloat("窗口圆角 / Window Rounding", &cfg.WindowRounding, 0.2f, 0, 20);
			ImGui::DragFloat("子面板圆角 / Child Panel Rounding", &cfg.ChildRounding, 0.2f, 0, 20);
			ImGui::DragFloat("控件圆角 / Widget Frame Rounding", &cfg.FrameRounding, 0.2f, 0, 20);
			ImGui::DragFloat("弹窗圆角 / Popup Window Rounding", &cfg.PopupRounding, 0.2f, 0, 20);
			ImGui::Spacing();

			ImGui::Text("边框粗细 / Border Thickness");
			ImGui::DragFloat("窗口边框 / Window Border Width", &cfg.WindowBorderSize, 0.1f, 0, 5);
			ImGui::DragFloat("控件边框 / Widget Frame Border Width", &cfg.FrameBorderSize, 0.1f, 0, 5);
			ImGui::DragFloat("弹窗边框 / Popup Border Width", &cfg.PopupBorderSize, 0.1f, 0, 5);
			ImGui::Spacing();

			ImGui::Text("内边距与间距 / Padding & Spacing");
			ImGui::DragFloat2("窗口内边距 / Window Inner Padding", &cfg.WindowPadding.x, 0.5f, 0, 50);
			ImGui::DragFloat2("控件内边距 / Widget Frame Padding", &cfg.FramePadding.x, 0.5f, 0, 30);
			ImGui::DragFloat2("控件间距 / Widget Item Spacing", &cfg.ItemSpacing.x, 0.5f, 0, 30);
			ImGui::DragFloat2("元素内部间距 / Inner Element Spacing", &cfg.ItemInnerSpacing.x, 0.5f, 0, 20);
			ImGui::Spacing();

			ImGui::Text("界面配色 / Interface Colors");
			ImGui::ColorEdit4("窗口背景色 / Window Background", &cfg.col_WindowBg.x);
			ImGui::ColorEdit4("控件默认底色 / Widget Normal Background", &cfg.col_FrameBg.x);
			ImGui::ColorEdit4("控件悬浮底色 / Widget Hover Background", &cfg.col_FrameBgHovered.x);
			ImGui::ColorEdit4("控件点击底色 / Widget Active Background", &cfg.col_FrameBgActive.x);
			ImGui::ColorEdit4("按钮常态底色 / Button Normal", &cfg.col_Button.x);
			ImGui::ColorEdit4("按钮悬浮底色 / Button Hover", &cfg.col_ButtonHovered.x);
			ImGui::ColorEdit4("按钮按下底色 / Button Active", &cfg.col_ButtonActive.x);
			ImGui::ColorEdit4("折叠标题常态 / Header Normal", &cfg.col_Header.x);
			ImGui::ColorEdit4("折叠标题悬浮 / Header Hover", &cfg.col_HeaderHovered.x);
			ImGui::ColorEdit4("折叠标题激活 / Header Active", &cfg.col_HeaderActive.x);
			ImGui::ColorEdit4("全局文字颜色 / Global Text Color", &cfg.col_Text.x);

			ImGui::Spacing();
			ImGui::Text("大小 / size");
			ImGui::DragFloat2("图片大小 / Image Size", &cfg.IMAGESIZE.x, 0.5f, 0, 50);
			ImGui::Checkbox("按钮大小跟随文字大小 / Button follow font size", &cfg.buttoniffollowfont);
			ButtonSIZE = ImVec2(-1, 0);
			if (!cfg.buttoniffollowfont) {
				ImGui::DragFloat2("按钮大小 / Button Size", &cfg.ButtonSIZE.x, 1, -1, 200);
				ButtonSIZE = ImVec2(cfg.ButtonSIZE.x, cfg.ButtonSIZE.x);
			}
			IMAGESIZE = ImVec2(cfg.IMAGESIZE.x, cfg.IMAGESIZE.x);

		}

		if (ImGui::CollapsingHeader("层级列表 / Hierarchy List View"))
		{
			ImGui::Checkbox("显示元素序号 / Show Item Index Number", &cfg.showItemIndex);
		//	ImGui::Checkbox("开启交互动画 / Enable Interactive Animation", &cfg.enableAnim);
		}

		if (ImGui::CollapsingHeader("导出与保存 / Export & Save Options"))
		{
			ImGui::Checkbox("导出代码附带id注释 / Export Code With id Comments", &cfg.exportWithComment);
			//ImGui::Checkbox("自动格式化代码缩进 / Auto Format Code Indent", &cfg.exportIndent);
			//ImGui::DragInt("单级缩进空格数 / Indent Space Count", &cfg.exportIndentSpace, 1, 2, 8);
		}

		if (ImGui::CollapsingHeader("调试 / Debug"))
		{
			ImGui::Checkbox("输出错误日志 / Output Error Log Messages", &cfg.enableLogError);

			ImGui::Spacing();
			if (ImGui::Button("重置全部设置 / Reset All Configuration", ImVec2(-1, 45)))
			{
				cfg.ResetAll();
			}
		}

		ImGui::Spacing();
		if (ImGui::Button("返回编辑界面 / Back To Editor View", ImVec2(-1, 55)))
		{
			openSetPopup = false;
			SaveSet();
		}

		ImGui::PopStyleVar(3);
		ImGui::EndPopup();
	}
}

bool SaveSet() {
	FILE* fp = fopen("./Sfaui.config", "wb");
	if (!fp)
	{
		Sfaui_log("SaveBinary fail: open file failed");
		return false;
	}

	size_t ret = fwrite(&g_SettingConfig, sizeof(SfauiSettingConfig), 1, fp);
	fflush(fp);
	fclose(fp);

	if (ret != 1)
	{
		Sfaui_log("写入配置失败");
		return false;
	}
	return true;
}

static std::string GetFlagCodeName(int bit, const FlagOption* list, size_t count, const char* prefix)
{
	for (size_t i = 0; i < count; i++)
	{
		if (list[i].bit == bit)
		{

			std::string label = list[i].label;
			size_t sp = label.find(' ');
			std::string name = label.substr(0, sp);
			return std::string(prefix) + name;
		}
	}

	std::ostringstream oss;
	oss << "(" << prefix << ")" << bit;
	return oss.str();
}


static std::string GetBitmaskCode(int mask, const FlagOption* list, size_t count, const char* prefix)
{
	std::vector<std::string> parts;
	for (size_t i = 0; i < count; i++)
	{
		int b = list[i].bit;
		if ((mask & b) != 0)
		{
			std::string label = list[i].label;
			size_t sp = label.find(' ');
			std::string name = label.substr(0, sp);
			parts.push_back(std::string(prefix) + name);
		}
	}
	if (parts.empty())
		return std::string(prefix) + "None";

	std::string res;
	for (size_t i = 0; i < parts.size(); i++)
	{
		if (i > 0) res += " | ";
		res += parts[i];
	}
	return res;
}

ImVec2 findpair_forcode = ImVec2(-1, -1);
std::vector<ImVec2>  pairforcode;
std::string MakeCode()
{
	SfauiSettingConfig& cfg = g_SettingConfig;
	std::stack<int> pairs;

	std::ostringstream oss;
	oss << "//本代码为伪代码，如果有  请补充参数 full the code  的字样，请用AI或手动查阅参数列表来修正\n";
	oss << "// This code is pseudocode. If you see the text \"请补充参数 full the code\", please refer to the parameter list manually or via AI to make corrections.\n";
	for (size_t i = 0; i < All_mRNA.size(); i++)
	{
		const auto& arr = All_mRNA[i];
		if (!arr[FUNCTIONNMAE].has_value() || arr[PARAMNUM].type() != typeid(int))
			continue;
		if (!arr[PARAMNUM].has_value() || arr[FUNCTIONNMAE].type() != typeid(std::string))
			continue;
         

		int paramCnt = any_cast<int>(arr[PARAMNUM]);
		std::string funcName = any_cast<std::string>(arr[FUNCTIONNMAE]);


		oss << funcName << "(";

		for (int p = 0; p < paramCnt; p++)
		{
			int idx = PARAMST + p;
			if (idx >= 20) break;
			if (!arr[idx].has_value()) continue;

			std::string arg = AnyToCodeStr(arr[idx], arr[VLENUM], p, paramCnt);
			if (arg.empty()) continue;

			if (p > 0) oss << ", ";
			oss << arg;
		}

		oss << ");";
		if(cfg.exportWithComment)
			oss << "  //"<< any_cast<std::string>(arr[REALID]) << "\n";
		else
			oss  << "\n";
	}
	return oss.str();
}



std::string AnyToCodeStr(const nonstd::any& val, const nonstd::any& layoutTypeAny, int paramIndex, int totalParam)
{
	if (!val.has_value())
		return "";

	AnyType tp = GetAnyType(val);
	switch (tp)
	{
	case AnyType_Empty:
		return "";

	case AnyType_Int:
	{
		int i = nonstd::any_cast<int>(val);
		int flagMask = 0;

		if (layoutTypeAny.type() == typeid(LLayout))
		{
			int curLay = static_cast<int>(nonstd::any_cast<LLayout>(layoutTypeAny));
			flagMask = CalcFlagMask(typeid(LLayout), curLay, paramIndex, totalParam);
		}
		else if (layoutTypeAny.type() == typeid(LView))
		{
			int curView = static_cast<int>(nonstd::any_cast<LView>(layoutTypeAny));
			flagMask = CalcFlagMask(typeid(LView), curView, paramIndex, totalParam);
		}

		if (flagMask & BIT_WINFLAG)
			return GetBitmaskCode(i, g_WindowFlagList, g_WindowFlagCount, "ImGuiWindowFlags_");

		else if (flagMask & BIT_TREEFLAG)
			return GetBitmaskCode(i, g_TreeFlagList, g_TreeFlagCount, "ImGuiTreeNodeFlags_");

		else if (flagMask & BIT_COLFLAG)
			return GetFlagCodeName(i, g_ImGuiColList, g_ImGuiColCount, "ImGuiCol_");

		else if (flagMask & BIT_STYLEVAR_VEC2)
			return GetFlagCodeName(i, g_StyleVarVec2List, g_StyleVarVec2Count, "ImGuiStyleVar_");

		else if (flagMask & BIT_STYLEVAR_FLOAT)
			return GetFlagCodeName(i, g_StyleVarFloatList, g_StyleVarFloatCount, "ImGuiStyleVar_");

		else if (flagMask & BIT_IMGUICOND)
			return GetFlagCodeName(i, g_ImGuiCondList, g_ImGuiCondCount, "ImGui");

		else if (flagMask & BIT_INPUTTEXTFLAG)
			return GetBitmaskCode(i, g_InputTextFlagList, g_InputTextFlagCount, "ImGuiInputTextFlags_");

		else if (flagMask & BIT_DRAGFLAG)
			return GetBitmaskCode(i, g_DragFlagList, g_DragFlagCount, "ImGuiDragFlags_");
		else
			return std::to_string(i);
	}

	case AnyType_Float:
	{
		float f = nonstd::any_cast<float>(val);
		std::ostringstream oss;
		oss << f;
		return oss.str();
	}

	case AnyType_Double:
	{
		double d = nonstd::any_cast<double>(val);
		std::ostringstream oss;
		oss << d;
		return oss.str();
	}

	case AnyType_Bool:
	{
		bool b = nonstd::any_cast<bool>(val);
		return b ? "true" : "false";
	}

	case AnyType_String:
	{
		std::string s = nonstd::any_cast<std::string>(val);
		return "\"" + s + "\"";
	}

	case AnyType_ImVec2:
	{
		ImVec2 v = nonstd::any_cast<ImVec2>(val);
		std::ostringstream oss;
		oss << "ImVec2(" << v.x << ", " << v.y << ")";
		return oss.str();
	}

	case AnyType_ImVec4:
	{
		ImVec4 v = nonstd::any_cast<ImVec4>(val);
		std::ostringstream oss;
		oss << "ImVec4(" << v.x << "," << v.y << "," << v.z << "," << v.w << ")";
		return oss.str();
	}

	case AnyType_ImU32:
	{
		ImU32 c = nonstd::any_cast<ImU32>(val);
		std::ostringstream oss;
		oss << "0x" << std::hex << c;
		return oss.str();
	}

	case AnyType_LLayout:
	{
		LLayout e = nonstd::any_cast<LLayout>(val);
		std::ostringstream oss;
		oss << "(LLayout)" << (int)e;
		return oss.str();
	}

	case AnyType_LView:
	{
		LView e = nonstd::any_cast<LView>(val);
		std::ostringstream oss;
		oss << "(LView)" << (int)e;
		return oss.str();
	}

	// case AnyType_Help_mRNA:
		 //return "";

	case AnyType_Unknown:
	default:
		return "请补充参数 full the code";
	}
}