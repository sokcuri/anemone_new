#pragma once
namespace SettingDlg
{
	INT_PTR CALLBACK    DlgProc(HWND, UINT, WPARAM, LPARAM);
	void				InitSection(HWND hWnd, HWND *hDlg);
	void				SwitchSection(HWND *hDlg, int id);
	INT_PTR CALLBACK    SectionProc(HWND, UINT, WPARAM, LPARAM);
}
