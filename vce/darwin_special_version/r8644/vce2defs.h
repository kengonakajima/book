#ifndef VCE_VCE2DEFS_H
#define VCE_VCE2DEFS_H

#include "vce2config.h"

namespace vce
{
	struct MemoryAllocator
	{
		virtual ~MemoryAllocator(){}
		virtual void *alloc(std::size_t sz)=0;
		virtual void free(void *p)=0;
		template <typename T,typename A>T*New(A&a)
		{
			T*t=(T*)alloc(sizeof(T));
			new(t)T(a);
			return t;
		}
		template <typename T>T*New()
		{
			T*t=(T*)alloc(sizeof(T));
			new(t)T();
			return t;
		}
		template <typename T>void Delete(T*t)
		{
			t->T::~T();
			free(t);
		}
	};

	template<typename T>
	struct Allocatable
	{
		T *t;
		MemoryAllocator *&cma;
		explicit Allocatable(MemoryAllocator *&ma):cma(ma){}
		~Allocatable(){}
		T&operator *(){return *t;}
		operator T*(){return t;}
		operator bool(){return t!=0;}
		T*operator->(){return t;}
		void operator=(Allocatable &a){}
	};

	template<typename T>
	struct ArrayPointer:
		public Allocatable<T>
	{
		std::size_t sz;
		typedef Allocatable<T> Super;
		explicit ArrayPointer(MemoryAllocator *&ma)
			:Super(ma)
		{
		}
		ArrayPointer&operator=(const ArrayPointer &aa)
		{
			this->t=aa.t;
			this->sz=aa.sz;
			return *this;
		}
		void New(std::size_t size)
		{
			this->sz=size;
			Super::t=(T*)Super::cma->alloc(sizeof(T)*sz);
			for(std::size_t i=0;i<sz;i++)
				new((void*)(Super::t+i))T;
		}
		void Delete()
		{
			for(std::size_t i=0;i<sz;i++)
				Super::t[i].T::~T();
			Super::cma->free(Super::t);
		}
		T&operator[](const int &i)
		{
			return Super::t[i];
		}
		void operator=(std::size_t n)
		{
			Super::t=(T*)n;
		}
	};

	template<typename T>
	struct Pointer
		:public Allocatable<T>
	{
		typedef Allocatable<T> Super;
		explicit Pointer(MemoryAllocator *&ma)
			:Super(ma)
		{
		}
		Pointer<T>&operator=(const Pointer<T>&a)
		{
			this->Super::t=a.Super::t;
			return *this;
		}
		void New()
		{
			Super::t=(T*)Super::cma->alloc(sizeof(T));
			new((void*)Super::t)T;
		}
		template<typename A>
		void New(A&a)
		{
			Super::t=(T*)Super::cma->alloc(sizeof(T));
			new((void*)Super::t)T(a);
		}
		void Delete()
		{
			Super::t->T::~T();
			Super::cma->free(Super::t);
		}
		void operator=(std::size_t n)
		{
			Super::t=(T*)n;
		}

	};

}

#include <string.h>

namespace vce
{
	namespace cs
	{
		struct setw
		{
			int width;
			setw(int width)
				:width(width)
			{
			}
		};
		struct setfill
		{
			int fill;
			setfill(int fill)
				:fill(fill)
			{
			}
		};
	}

	template<int LocalSize,typename T,typename Allocator >
	struct charstream
	{
		T local[LocalSize];
		bool usinglocal;
		T *buf;
		size_t start;
		size_t end;
		size_t bufsize;
		Allocator altr;
		int width;
		T fill;
		template<int Size>
		T* ntos(T (&inout)[Size],int n)
		{
			inout[Size-1]=0;
			int keta=0;
			if(n)
			{
				for(;n;n/=10)
				{
					keta++;
					inout[Size-1-keta]=static_cast<T>('0'+(n%10));
				}
			}
			else
			{
				inout[Size-2]='0';
				keta=1;
			}
			while(width>keta)
			{
				keta++;
				inout[Size-1-keta]=fill;
			}
			return inout+Size-1-keta;
		}
	public:
		charstream(Allocator allocator=Allocator())
			:usinglocal(true)
			,buf(local)
			,start(0)
			,end(0)
			,bufsize(LocalSize)
			,altr(allocator)
			,width(0)
			,fill(' ')
		{
		}
		size_t size()
		{
			return end-start;
		}
		charstream& operator<<(const T *str)
		{
			size_t len=strlen(str);
			if(size()+len+1>bufsize)
			{
				while(size()+len+1>bufsize)
					bufsize*=2;
				T *temp=altr.allocate(bufsize);
				memcpy(temp,buf+start,size()*sizeof(T));
				start=0;
				if(usinglocal)
					usinglocal=false;
				else
					altr.deallocate(buf);
				buf=temp;
			}
			memcpy(buf+end,str,len*sizeof(T));
			end+=len;
			return *this;
		}
		charstream& operator<<(int n)
		{
			T keta[13];
			return *this<<(ntos(keta,n));
		}
		charstream& operator<<(const cs::setw &w)
		{
			this->width=w.width;
			return *this;
		}
		charstream& operator<<(const cs::setfill &f)
		{
			this->fill=(T)f.fill;
			return *this;
		}
		const T* c_str()
		{
			buf[end]=0;
			return buf+start;
		}
	};
}

#endif//VCE_VCE2DEFS_H
