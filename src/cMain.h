#pragma once
#include "taskBarIcon.h"

class cMain : public wxFrame
{
public:
	cMain();
	~cMain();

public:
	wxPanel *mainPanel = nullptr;
	wxButton *btn_Inject = nullptr;
	wxButton *btn_Hide = nullptr;
	wxButton *btn_Select = nullptr;
	wxTextCtrl *txt_Name = nullptr;
	wxTextCtrl *txt_Delay = nullptr;
	wxTextCtrl *txt_Path = nullptr;
	wxBoxSizer *sizer = nullptr;
	wxCheckBox *check_Custom = nullptr;
	wxCheckBox *check_Auto = nullptr;

	wxFileDialog *openDialog = nullptr;
	taskBarIcon taskBarControl;
	wxNotificationMessage *notification = nullptr;
	wxTimer autoInjectTimer;
	DWORD lastAutoInjectedProcId = 0;

public:
	static void OnInjectButtonExecute(wxCommandEvent &evt, cMain *ref);
	void OnInjectButton(wxCommandEvent &evt);
	void OnHideButton(wxCommandEvent &evt);
	void OnSelectButton(wxCommandEvent &evt);

	void OnCustomCheckBox(wxCommandEvent &evt);
	void OnAutoCheckBox(wxCommandEvent &evt);
	void OnAutoInjectTimer(wxTimerEvent& evt);

	void onTaskBarDClick(wxCommandEvent &evt);

	wxDECLARE_EVENT_TABLE();

public:
	void disableAutoInject();
	bool InjectCurrentTarget(bool autoInjectMode, DWORD* injectedProcId = nullptr);
	int GetValidatedDelaySeconds();
	void SaveCurrentConfig();
	friend taskBarIcon::taskBarIcon();
};
