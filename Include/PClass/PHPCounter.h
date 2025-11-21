#ifndef _PHPCOUNTER_H_
#define _PHPCOUNTER_H_

class 
//#ifdef _MAKEDLL
  AFX_EXT_CLASS
//#endif
PHPCounter
{
  private:
    LARGE_INTEGER miStartPerformanceCount;
    LARGE_INTEGER miEndPerformanceCount;
    LARGE_INTEGER miPerformanceFrequency;

  public:
    PHPCounter();
    ~PHPCounter();

    void StartCounter();
    void EndCounter();
    
    double GetMillisecond();    
};

#endif