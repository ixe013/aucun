#ifndef __TOOLHELP_ITERATOR_H__GSEGUIN_AT_BIGFOOT_COM__
#define __TOOLHELP_ITERATOR_H__GSEGUIN_AT_BIGFOOT_COM__

/*

  Please go to http://pages.infinit.net/gseguin for more information.

  I hope you find this code usefull. You can redistribute this file
  with/on anything, as long as it remains free and unmodified. If you
  find a bug in it, please advise me so that everyone can benefit
  from it.

  Please drop me a line about anything good or bad about this code.

  (Vous pouvez aussi m'écrire en français).

  Guillaume Seguin, gseguin@bigfoot.com


  Revision history
  ----------------
  15 oct 2001  - Integrated code to enable debug priviledge, made the
                 code compile at warning level 4.
  05 oct 2001  - Create a .cpp for the helper files.
  02 oct 2001  - Initial release.


  How to use it
  -------------
  Create the begin or first iterator by passing a parameter to the
  constructor. The end iterator is created when you don't give any
  parameters, but there is a static member named End in every class
  you can use directly. This code lists every modules of every
  running process.

  for(CProcessIterator process(0); process != CProcessIterator::End; ++process)
  {
    for(CModuleIterator module(process.th32ProcessID); module != CProcessIterator::End; ++module)
    {
      printf("PID = %d, Module name %s\n", process.th32ProcessID, module.szExePath);
    }
  }


*/

#include <tlhelp32.h>
#include <iterator>

#define MAX_PRIV_NAME 256

class _RefCountedPriviledgeHolder
{
    protected:
        DWORD m_Count;
        LPCTSTR m_Priv;

        BOOL EnablePrivilege(HANDLE hToken, LPCTSTR szPrivName, BOOL fEnable);
        BOOL EnablePrivilegeForCurrentProcess(LPCTSTR szPrivName, BOOL fEnable);

    public:
        void Release();
        void AddRef();
        _RefCountedPriviledgeHolder(LPCTSTR szPrivName);
        ~_RefCountedPriviledgeHolder();
};

template<typename T>
struct CToolHelpData
{
    virtual BOOL FirstData(HANDLE, T*) = 0;
    virtual BOOL NextData(HANDLE, T*) = 0;
    virtual void InitData(T&);
    virtual void CleanData(T&);

    virtual bool Compare(const T&, const T&) const = 0;

    virtual HANDLE CreateSnapshot(DWORD, DWORD);
};

template<typename T>
void CToolHelpData<T>::InitData(T& data)
{
    data.dwSize = sizeof T;
}

template<typename T>
void CToolHelpData<T>::CleanData(T& data)
{
    static const T t = {0};
    data = t;
    data.dwSize = sizeof T;
}

template<typename T>
HANDLE CToolHelpData<T>::CreateSnapshot(DWORD flags, DWORD pid)
{
    return CreateToolhelp32Snapshot(flags, pid);
}


class CToolHelpProcess : public CToolHelpData<PROCESSENTRY32>
{
    public:
        BOOL FirstData(HANDLE, PROCESSENTRY32*);
        BOOL NextData(HANDLE, PROCESSENTRY32*);

        bool Compare(const PROCESSENTRY32&, const PROCESSENTRY32&) const;
};

class CToolHelpModule : public CToolHelpData<MODULEENTRY32>
{
    protected:
        //We need debug priviledges to list modules from a process
        //launched by another user.
        static _RefCountedPriviledgeHolder m_Debug;
        bool mDebugPrivEnabled;

    public:
        CToolHelpModule(bool debugpriv = false);
        ~CToolHelpModule();

        BOOL FirstData(HANDLE, MODULEENTRY32*);
        BOOL NextData(HANDLE, MODULEENTRY32*);

        bool Compare(const MODULEENTRY32&, const MODULEENTRY32&) const;
};


class CToolHelpThread : public CToolHelpData<THREADENTRY32>
{
    public:
        BOOL FirstData(HANDLE, THREADENTRY32*);
        BOOL NextData(HANDLE, THREADENTRY32*);

        bool Compare(const THREADENTRY32&, const THREADENTRY32&) const;
};

template<typename T>
class CToolHelpIteratorFilter
{
    public:
        enum
        {
            KeepIt,
            LooseIt,
            Stop
        };

        virtual int Filter(const T&)
        {
            return KeepIt;
        }
};


template<typename T, typename H, DWORD F>
class CToolHelpIterator :
    public std::iterator<std::forward_iterator_tag, T>
{
    protected:
        HANDLE m_ToolSnap;
        DWORD m_SnapFlag;
        H m_Helper;
        T m_Data;

        CToolHelpIteratorFilter<T> *m_Filter;

        typedef BOOL (CToolHelpData<T>::*NextOperation)(HANDLE, T*);

        NextOperation m_Next;

        void Start(DWORD pid);
        void Finish();

        void SetFilter(CToolHelpIteratorFilter<T> *);
        void ClearFilter();

        //Those function are hidden because there is no way for me to know where the
        //iterator is, and thus I can't make a copy of it. Construct a new one instead.
        //Unfortunatly, it prevents you from usign algorithm like stdfor_each.
        CToolHelpIterator(const CToolHelpIterator& thi)
        {
            operator = (thi);
        }
        CToolHelpIterator& operator = (const CToolHelpIterator& thi) {}

    public:
        //Begin-like iterator
        CToolHelpIterator(DWORD pid, CToolHelpIteratorFilter<T> *f = 0, DWORD flags = F)
            : m_SnapFlag(flags), m_ToolSnap(INVALID_HANDLE_VALUE), m_Filter(f)
        {
            if(m_Filter == 0)
            {
                SetFilter(new CToolHelpIteratorFilter<T>);
            }

            m_Next = &CToolHelpData<T>::FirstData;
            Start(pid);
        }
        //End like iterator
        CToolHelpIterator() : m_SnapFlag(0), m_ToolSnap(INVALID_HANDLE_VALUE), m_Filter(new CToolHelpIteratorFilter<T>)
        {
            Finish();
        }

        ~CToolHelpIterator()
        {
            Finish();
            ClearFilter();
        }

        const T& operator*() const
        {
            return m_Data;
        }
        const T* operator->() const
        {
            return &m_Data;
        }

        bool operator == (const CToolHelpIterator<T, H, F> &di) const;
        bool operator != (const CToolHelpIterator<T, H, F> &di) const;

        const T& operator++();
        T operator++(int);

        const T& operator+=(int);

        static const CToolHelpIterator<T, H, F> End;
};

template<typename T, typename H, DWORD F>
void CToolHelpIterator<T, H, F>::Start(DWORD pid)
{
    Finish(); //safety
    m_Helper.InitData(m_Data);
    m_ToolSnap = m_Helper.CreateSnapshot(m_SnapFlag, pid);

    if(m_ToolSnap != INVALID_HANDLE_VALUE)
    {
        operator++();
    }
}

template<typename T, typename H, DWORD F>
void CToolHelpIterator<T, H, F>::Finish()
{
    if(m_ToolSnap != INVALID_HANDLE_VALUE)
    {
        CloseHandle(m_ToolSnap);
        m_ToolSnap = INVALID_HANDLE_VALUE; //paranoia
    }

    m_Helper.CleanData(m_Data);
}

template<typename T, typename H, DWORD F>
bool CToolHelpIterator<T, H, F>::operator == (const CToolHelpIterator<T, H, F> &pi) const
{
    bool are_equal;

    if((m_ToolSnap == INVALID_HANDLE_VALUE) && (pi.m_ToolSnap == INVALID_HANDLE_VALUE))
    {
        //Comparing to 'end' iterator
        are_equal = true;
    }
    else if((m_ToolSnap == INVALID_HANDLE_VALUE) || (pi.m_ToolSnap == INVALID_HANDLE_VALUE))
    {
        are_equal = false;
    }
    else
    {
        are_equal = m_Helper.Compare(m_Data, pi.m_Data);
    }

    return are_equal;
}

template<typename T, typename H, DWORD F>
bool CToolHelpIterator<T, H, F>::operator != (const CToolHelpIterator<T, H, F> &di) const
{
    return !operator == (di);
}

template<typename T, typename H, DWORD F>
const T& CToolHelpIterator<T, H, F>::operator++()
{
    return operator += (1);
}

template<typename T, typename H, DWORD F>
T CToolHelpIterator<T, H, F>::operator++(int)
{
    T old = m_Data;
    operator ++ ();
    return old;
}

template<typename T, typename H, DWORD F>
const T& CToolHelpIterator<T, H, F>::operator+=(int n)
{
    int whatnext = CToolHelpIteratorFilter<T>::KeepIt;

    for(int i = 0; i < n; ++i)
    {
        do
        {
            if((m_Helper.*m_Next)(m_ToolSnap, &m_Data) == FALSE)
            {
                Finish();
                break; //no point going on
            }

            //This is redondant except for the first call
            m_Next = &CToolHelpData<T>::NextData;
        }
        while((whatnext = m_Filter->Filter(m_Data)) == CToolHelpIteratorFilter<T>::LooseIt);

        if(whatnext == CToolHelpIteratorFilter<T>::Stop)
        {
            Finish();
            break; //no point going on
        }
    }

    return m_Data;
}

template<typename T, typename H, DWORD F>
void CToolHelpIterator<T, H, F>::SetFilter(CToolHelpIteratorFilter<T> *f)
{
    ClearFilter();
    m_Filter = f;
}

template<typename T, typename H, DWORD F>
void CToolHelpIterator<T, H, F>::ClearFilter()
{
    if(m_Filter)
    {
        delete m_Filter;
        m_Filter = 0;
    }
}

class CSingleProcessThreadIteratorFilter : public CToolHelpIteratorFilter<THREADENTRY32>
{
    protected:
        DWORD m_PID;
    public:
        CSingleProcessThreadIteratorFilter(DWORD pid = GetCurrentProcessId()) : m_PID(pid) {}

        int Filter(const THREADENTRY32& entry)
        {
            return (entry.th32OwnerProcessID == m_PID) ? KeepIt : LooseIt;
        }
};


typedef CToolHelpIterator<PROCESSENTRY32, CToolHelpProcess, TH32CS_SNAPPROCESS> CProcessIterator;
typedef CToolHelpIterator<MODULEENTRY32, CToolHelpModule, TH32CS_SNAPMODULE> CModuleIterator;
typedef CToolHelpIterator<THREADENTRY32, CToolHelpThread, TH32CS_SNAPTHREAD> CThreadIterator;


#endif
