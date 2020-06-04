#pragma once
#include <imgui.h>
#include <array>
#include <optional>
#include <fstream>
#include <nlohmann/json.hpp>


// TODO: detail structure of .json so it will be correctly used/read
template<class _Func, typename _T = void>
class ImGuiConsole
{
	// TODO: add 3214 commands for text completion/suggestions (store in text file and load on init)
	// TODO: add a console log (hook the games console output stream)
	//const std::string path = R"(C:\Users\coxtr\source\repos\COD7 Scripting\COD7 Scripting\commands_test.json)";

public:
	_Func func;
	using ForwardExecuteFunction = std::function<_T(ImGuiConsole<_Func, _T>*)>; // give a more descriptive name that will autocomplete faster
	ForwardExecuteFunction ExecuteCommand;

private:
	char inputBuf[256];
	ImVector<char*> items;
	ImVector<char*> history;
	int historyPos; // -1: new line, 0..History.Size-1 browsing history
	int numberOfCommands;

	using json = nlohmann::json;
	json j;

	// Portable helpers
	static int   Stricmp(const char* s1, const char* s2) { int d; while ((d = toupper(*s2) - toupper(*s1)) == 0 && *s1) { s1++; s2++; } return d; }
	static int   Strnicmp(const char* s1, const char* s2, int n) { int d = 0; while (n > 0 && (d = toupper(*s2) - toupper(*s1)) == 0 && *s1) { s1++; s2++; n--; } return d; }
	static char* Strdup(const char* s) { size_t len = strlen(s) + 1; void* buf = malloc(len); IM_ASSERT(buf); return (char*)memcpy(buf, (const void*)s, len); }
	static void  Strtrim(char* s) { char* str_end = s + strlen(s); while (str_end > s && str_end[-1] == ' ') str_end--; *str_end = 0; }


public:

	/*
	cmdFunc: optional pointer to function to execute
	forward: function used to call the cmdFunc, only called if cmdFunc != nullptr
	commandFile: path to a .json file containing parameters. See ImGuiConsole.h:8 for how to structure the .json file
	*/
	ImGuiConsole(_Func cmdFunc = nullptr, ForwardExecuteFunction forward = {}, const char* commandFilePath = nullptr) :
		func{ cmdFunc }, ExecuteCommand{ forward }, inputBuf{ 0 }, items{}, history{}, historyPos{ -1 }
	{
		if (commandFilePath)
		{
			std::ifstream jsonFile(commandFilePath);
			j = json::parse(jsonFile); // can throw
			numberOfCommands = j["command_count"];
		}

	}

	~ImGuiConsole()
	{
		for (char* str : history) delete str;
	}

	int TextEditCallback(ImGuiTextEditCallbackData* data)
	{
		const int prevHistPos = historyPos;
		switch (data->EventFlag)
		{
		case ImGuiInputTextFlags_CallbackHistory:
			if (history.Size == 0) return 0; // if there is nothing in the history, just return
			if (data->EventKey == ImGuiKey_UpArrow)
			{
				if (historyPos == -1) // new line
					historyPos = history.Size - 1; // last line was added via push_back, so size-1 is the last element
				else if (historyPos > 0)
					historyPos--; // go further back in the history
			}
			else if (data->EventKey == ImGuiKey_DownArrow)
			{
				if (historyPos != -1) // if not new line (nothing is "below" the newest line)
					if (++historyPos >= history.Size) // if new history pos is larger than the size, we are back to the new line
						//TODO: restore the new line if an arrow key was pressed after user typed something
						historyPos = -1;
			}

			if (prevHistPos != historyPos)
			{
				const char* history_str = (historyPos >= 0) ? history[historyPos] : "";
				data->DeleteChars(0, data->BufTextLen);
				data->InsertChars(0, history_str);
				/*data->CursorPos = data->SelectionStart = data->SelectionEnd = data->BufTextLen = (int)snprintf(data->Buf, (size_t)data->BufSize, "%s", (historyPos >= 0) ? history[historyPos] : "");
				data->BufDirty = true;*/
			}
			break;
		default:
			break;


		}
		return 0;
	}

	std::optional<char*> Draw(const char* title, bool* p_open = (bool*)0)
	{
		bool enterPressed = false;
		ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
		if (!ImGui::Begin(title, p_open))
		{
			ImGui::End();
			return {};
		}

		// As a specific feature guaranteed by the library, after calling Begin() the last Item represent the title bar. So e.g. IsItemHovered() will return true when hovering the title bar.
		// Here we create a context menu only available from the title bar.
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Close Console"))
				*p_open = false;
			ImGui::EndPopup();
		}

		ImGui::TextWrapped("Console for issuing commands to the game via CBuf_AddText");
		ImGui::TextWrapped("Enter 'HELP' for help, press TAB to use text completion.(not implemented)");
		ImGui::Separator();

		auto inputFlags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory;
		auto forwardingCallback = [](ImGuiTextEditCallbackData* data) -> int {
			ImGuiConsole* self = static_cast<ImGuiConsole*>(data->UserData);
			return self->TextEditCallback(data);
		};

		bool reclaimFocus = false;
		if (ImGui::InputText(title, inputBuf, IM_ARRAYSIZE(inputBuf), inputFlags, forwardingCallback, (void*)this))
		{
			history.push_back(Strdup(inputBuf));
			ExecuteCommand(this);
			memset(inputBuf, 0, sizeof(inputBuf));
			reclaimFocus = true;
			enterPressed = true;
		}

		// Demonstrate keeping focus on the input box
		ImGui::SetItemDefaultFocus();
		if (reclaimFocus)
			ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget

		ImGui::End();
		if (enterPressed) return inputBuf;
		else return {};
	}

	char* GetCommand()
	{
		if (historyPos == -1) return inputBuf;
		else if (historyPos > 0 && historyPos < history.Size) return history[historyPos];
		else return {};
	}

};

