#pragma once



class IInterface  : public CWnd
{
public:
	IInterface() {};
	virtual ~IInterface() {};

public:
	virtual int Initialize() = 0;
	virtual int UnInitialize() = 0;
	virtual BOOL IsInitialized() const = 0;
	virtual int ReleaseSingleton() = 0;

};

// CMIInterface

class AFX_EXT_CLASS CMIInterface : public CWnd
{
	DECLARE_DYNAMIC(CMIInterface)

public:
	CMIInterface();
	virtual ~CMIInterface();

protected:
	DECLARE_MESSAGE_MAP()

public:
	int Initialize();
	int UnInitialize();
	BOOL IsInitialized() const;

protected:
	BOOL	m_bInitialized;

	IInterface* m_Interface;
};

