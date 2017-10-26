
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include "mgncsconfig.h"
#include "mcommon.h"
#include "mobject.h"
#include "mem-slab.h"

extern BOOL ncsRemoveObjectBindProps(mObject *obj);

static void mObject_construct(mObject* self, DWORD addData)
{
    if (self) {
        self->objRefCount = 0;
        self->objStatus = 0;
    }
}

static void mObject_destroy(mObject* self)
{
	ncsRemoveObjectBindProps(self);
	ncsRemoveEventSource(self);
	ncsRemoveEventListener(self);
}

static DWORD mObject_hash(mObject *self)
{
	return (DWORD)self;
}

static const char* mObject_toString(mObject *self, char* str, int max)
{
	if(!str)
		return NULL;

	snprintf(str, max, "NCS Object %s[@%p]", TYPENAME(self),self);
	return str;
}

static int mObject_addRef(mObject *self)
{
    if (self)
        return ++(self->objRefCount);
    return -1;
}

static int mObject_release(mObject *self)
{
    if (self) {
        --(self->objRefCount);
        if (NCS_OBJ_TODEL(self) && self->objRefCount <= 0) {
            //delete object 
            if (self->_class) {
                _c(self)->destroy(self);
                free(self);
            }
            return 0;
        }
        return self->objRefCount;
    }
    return -1;
}

void ncsRefObjFreeSpecificData(DWORD key, DWORD value)
{
	mObject* obj = (mObject*)value;
	if(obj)
		_c(obj)->release(obj);
}


static mObjectClass* mObjectClassConstructor(mObjectClass* _class)
{
	_class->super = NULL;
	_class->typeName = "mObject";
	_class->objSize = sizeof(mObject);

	CLASS_METHOD_MAP(mObject, construct)
	CLASS_METHOD_MAP(mObject, destroy)
	CLASS_METHOD_MAP(mObject, hash)
	CLASS_METHOD_MAP(mObject, toString)
	CLASS_METHOD_MAP(mObject, addRef)
	CLASS_METHOD_MAP(mObject, release)
	return _class;
}

mObjectClass Class(mObject) = {
	(PClassConstructor)mObjectClassConstructor
};

mObject * newObject(mObjectClass *_class)
{
	mObject * obj;

	if(_class == NULL)
		return NULL;

	obj = (mObject*)calloc(1, _class->objSize);

	if(!obj)
		return NULL;

	return initObject(obj, _class);
}

void deleteObject(mObject *obj)
{
	if(obj == NULL || obj->_class == NULL)
		return;

    if (obj->objRefCount > 0) {
        obj->objStatus |= NCSF_OBJECT_TODEL;
        return;
    }

	_c(obj)->destroy(obj);
	free(obj);
}

mObject* initObject(mObject* pobj, mObjectClass* _class) {
	IInterface* piobj;
	IInterfaceVTable* _ivtable;
	int next_intf_offset ;
	pobj->_class = _class;
    pobj->objRefCount = 0;
    pobj->objStatus = 0;

	next_intf_offset = _class->intfOffset;
	while(next_intf_offset > 0)
	{
		_ivtable = (IInterfaceVTable*)((unsigned char*)_class + next_intf_offset);
		piobj = (IInterface*)((unsigned char*)pobj + _ivtable->_obj_offset);
		piobj->_vtable = _ivtable;	
		next_intf_offset = _ivtable->_next_offset;
	}

	return pobj;
}


/////////////////////////////////////////////////////
//
//

mObjectClass *ncsSafeCastClass(mObjectClass* clss, mObjectClass* castCls)
{
	mObjectClass * clssSuper;
	if(clss == castCls)
		return NULL;

	clssSuper = clss;
	while(clssSuper && clssSuper != castCls)
		clssSuper = clssSuper->super;

	return clssSuper?clss:NULL;
}

mObject* ncsSafeCast(mObject* obj, mObjectClass *clss)
{
	mObjectClass * objClass;
	if(obj == NULL || clss == NULL)
		return NULL;

	objClass = _c(obj);

	while(objClass && objClass != clss)
		objClass = objClass->super;

	return objClass?obj:NULL;
}

BOOL ncsInstanceOf(mObject *object, mObjectClass* clss)
{
	mObjectClass* objClss;
	if(object == NULL || clss == NULL)
		return FALSE;

	objClss = _c(object);

	while(objClss && clss != objClss){
		objClss = objClss->super;
	}

	return objClss != NULL;
}

int ncsParseConstructParams(va_list args, const char* signature, ...)
{
	va_list params;
	int argc;
	int i;

	if(GET_ARG_COUNT(args) <= 0)
		return 0;

	argc = va_arg(args, int);
    if(argc <= 0)
        return 0;

	va_start(params, signature);
	i = 0;
	while(i < argc)
	{
		switch(signature[i])
		{
		case 'd': //double
			*(va_arg(params, double*)) = va_arg(args, double);
			break;
		case 'f': //float
			*(va_arg(params, float*)) = (float)va_arg(args, double);
			break;
		case 'i': //integer
			*(va_arg(params, int*)) = va_arg(args, int);
			break;
		case 's': //const char*
			*(va_arg(params, const char**)) = va_arg(args, const char*);
			break;
		case 'u': //unsigned int
			*(va_arg(params, unsigned int*)) = va_arg(args, unsigned int);
			break;
		case 'p': //void *
			*(va_arg(params, void**)) = va_arg(args, void*);
			break;
		default:
			*(va_arg(params, void**)) = va_arg(args, void*);
		}
		
		i ++;
	}

	va_end(params);

	return i;
}




