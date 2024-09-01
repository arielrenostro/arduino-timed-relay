#ifndef IBaseComponent_C
#define IBaseComponent_C

class IBaseComponent
{
public:
    virtual ~IBaseComponent() {};
    virtual bool setup() { return false; };
};

#endif