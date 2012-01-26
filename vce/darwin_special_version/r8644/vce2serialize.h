
#ifdef _VCE_GEN_SERIALIZE_DEFINED
# error "old gen-made code is used. please re-generate them"
#endif

#ifndef VCE_VCE2SERIALIZE_H
#define VCE_VCE2SERIALIZE_H

#include <vector>
#include <map>
#include <string>
#include "vce2.h"

#pragma once

namespace vce_gen_serialize
{

	struct pack
	{
		vce::VUint8 *first;
		vce::VUint8 *last;

		pack() {}
		pack(vce::VUint8* f, vce::VUint8 *l) : first(f), last(l) {}
		pack(vce::VUint8* f, size_t sz) : first(f), last(f+sz) {}

		bool empty() const { return first == last; }
		size_t size() const { return last - first; }

		template<typename Any>inline bool DirectPush(const Any &src)
		{
            if(first+sizeof(Any)>last)
				return false;
#ifdef VCE_BIG_ENDIAN
			for (int i=0; i<sizeof(Any); ++i) {
				*(first + sizeof(Any) - 1 - i) 
					= *(reinterpret_cast<const vce::VUint8*>(&src) + i);
			}
#elif defined(VCE_ARCH_ARM)
			for (int i=0; i<sizeof(Any); ++i) {
				*(first+i) = *(reinterpret_cast<const vce::VUint8*>(&src) + i);
			}

#elif defined(VCE_ARCH_MIPS)
			for (int i=0; i<sizeof(Any); ++i) {
				*(first+i) = *(reinterpret_cast<const vce::VUint8*>(&src) + i);
			}
#else
			*((Any*)(first))=src;
#endif
			first+=sizeof(src);
			return true;
		}
		template<typename Any>inline bool DirectPull(Any &dest)
		{
            if(first+sizeof(Any)>last)
				return false;
#ifdef VCE_BIG_ENDIAN
			for (int i=0; i<sizeof(Any); ++i) {
				*(reinterpret_cast<vce::VUint8*>(&dest) + sizeof(Any) - 1 - i)
					= *(first + i);
			}
#elif defined(VCE_ARCH_ARM)
			for (int i=0; i<sizeof(Any); ++i) {
				*(reinterpret_cast<vce::VUint8*>(&dest) + i) = *(first+i);
			}
#elif defined(VCE_ARCH_MIPS)
			for (int i=0; i<sizeof(Any); ++i) {
				*(reinterpret_cast<vce::VUint8*>(&dest) + i) = *(first+i);
			}
#else
			dest=*((Any*)(first));
#endif
			first+=sizeof(dest);
			return true;
		}
	};
	inline bool Push(const vce::VSint64 &src,pack &buf){return buf.DirectPush(src);}
	inline bool Push(const vce::VUint64 &src,pack &buf){return buf.DirectPush(src);}
	inline bool Push(const vce::VSint32 &src,pack &buf){return buf.DirectPush(src);}
	inline bool Push(const vce::VUint32 &src,pack &buf){return buf.DirectPush(src);}
	inline bool Push(const vce::VSint16 &src,pack &buf){return buf.DirectPush(src);}
	inline bool Push(const vce::VUint16 &src,pack &buf){return buf.DirectPush(src);}
	inline bool Push(const vce::VSint8 &src,pack &buf){return buf.DirectPush(src);}
	inline bool Push(const vce::VUint8 &src,pack &buf){return buf.DirectPush(src);}
	inline bool Push(const float &src,pack &buf){return buf.DirectPush(src);}
	inline bool Push(const double &src,pack &buf){return buf.DirectPush(src);}
	inline bool Pull(vce::VSint64 &dest,pack &buf){return buf.DirectPull(dest);}
	inline bool Pull(vce::VUint64 &dest,pack &buf){return buf.DirectPull(dest);}
	inline bool Pull(vce::VSint32 &dest,pack &buf){return buf.DirectPull(dest);}
	inline bool Pull(vce::VUint32 &dest,pack &buf){return buf.DirectPull(dest);}
	inline bool Pull(vce::VSint16 &dest,pack &buf){return buf.DirectPull(dest);}
	inline bool Pull(vce::VUint16 &dest,pack &buf){return buf.DirectPull(dest);}
	inline bool Pull(vce::VSint8 &dest,pack &buf){return buf.DirectPull(dest);}
	inline bool Pull(vce::VUint8 &dest,pack &buf){return buf.DirectPull(dest);}
	inline bool Pull(float &dest,pack &buf){return buf.DirectPull(dest);}
	inline bool Pull(double &dest,pack &buf){return buf.DirectPull(dest);}

	template<class Packer>
	inline bool Push(const vce::VBOOL &src,Packer &buf)
	{
		if(src)
			return Push((vce::VUint8)1, buf);
		else
			return Push((vce::VUint8)0, buf);
	}

	template<class Packer>
	inline bool Pull(vce::VBOOL &dest,Packer &buf)
	{
		vce::VUint8 c;
		if(Pull(c, buf))
		{
			dest=c==1;
			return true;
		}
		else
			return false;
	}

	inline bool Push(const char *src,pack &buf,vce::VUint32 src_buf_len)
	{
		if(NULL==src)return false;
		const char *end=(const char*)std::memchr(src,'\0',src_buf_len);
		if(!end)return false;
		end++;
        if(buf.first+(end-src)>buf.last)return false;
        std::memcpy(buf.first,src,end-src);
		buf.first+=end-src;
		return true;
	}

	inline bool Pull(char *dest,pack &buf,vce::VUint32 dest_buf_len)
	{
		int len=(int)dest_buf_len<(int)(buf.last-buf.first)?(int)dest_buf_len:(int)(buf.last-buf.first);
		if(len<0)
			return false;
		unsigned char *end=(unsigned char*)std::memchr(buf.first,'\0',len);
		if(!end)
			return false;
		end++;
		std::memcpy(dest,buf.first,end-buf.first);
		buf.first=end;
        return true;
	}

	// XXX: broken due to endianness incompatiblility when Any == wchar
	template<typename Any, class Alloc>
	inline bool Push(const std::basic_string<Any, std::char_traits<Any>, Alloc> &src,pack &buf)
	{
		size_t len=(src.length()+1)*sizeof(Any);
		if(buf.first+len>buf.last)
			return false;
		std::memcpy(buf.first,src.data(),len);
		buf.first+=len;
		return true;
	}

	template<typename Any, class Alloc>
	inline bool Pull(std::basic_string<Any, std::char_traits<Any>, Alloc> &dest,pack &buf)
	{
		size_t len=0;
		for(;;)
		{
			if(buf.first+len+sizeof(Any)>buf.last)
				return false;
			if(*((Any*)(buf.first+len))==0)
				break;
			len+=sizeof(Any);
		}
		len+=sizeof(Any);
		dest=(Any*)(buf.first);
		buf.first+=len;
		return true;
	}

	template<typename Any,typename Bny,typename Packer>
	inline bool Push(const std::pair<Any,Bny> &src,Packer &buf)
	{
		if(!Push(src.first,buf))
			return false;
		return Push(src.second,buf);
	}

	template<typename Any,typename Bny,typename Packer>
	inline bool Pull(std::pair<Any,Bny> &dest,Packer &buf)
	{
		if(!Pull(dest.first,buf))
			return false;
		return Pull(dest.second,buf);
	}

	template<typename Any,typename Bny,typename Comp,typename Alloc,typename Packer>
	inline bool Push(const std::map<Any,Bny,Comp,Alloc> &src,Packer &buf)
	{
		vce::VUint32 map_size=static_cast<vce::VUint32>(src.size());
		if(!Push(map_size,buf))
			return false;

		for(typename std::map<Any,Bny,Comp,Alloc>::const_iterator i=src.begin();i!=src.end();i++)
			if(!Push(*i,buf))
				return false;
		return true;
	}

	template<typename Any,typename Bny,typename Comp,typename Alloc,typename Packer>
	inline bool Pull(std::map<Any,Bny,Comp,Alloc> &dest,Packer &buf)
	{
		vce::VUint32 dest_size;
		if (!Pull(dest_size, buf))
			return false;

		std::pair<Any,Bny> pa;
		while(dest_size--)
		{
			if(!Pull(pa,buf))
				return false;
			dest.insert(pa);
		}

		return true;
	}

	template<typename Any,typename Alloc,typename Packer>
	inline bool Push(const std::vector<Any,Alloc> &src,Packer &buf)
	{
		vce::VUint32 vec_size=static_cast<vce::VUint32>(src.size());
		if(!Push(vec_size,buf))
			return false;

		vce::VUint32 src_size=(vce::VUint32)src.size();
		for(vce::VUint32 i=0;i<src_size;i++)
			if(!Push(src[i],buf))
				return false;
		return true;
	}

	template<typename Any,typename Alloc,typename Packer>
	inline bool Pull(std::vector<Any,Alloc> &dest,Packer &buf)
	{
		vce::VUint32 dest_size,nowsize=0;
		if (!Pull(dest_size, buf))
			return false;

		while(dest_size--)
		{
			dest.resize(nowsize+1);
			if(!Pull(dest[nowsize],buf))
				return false;
            nowsize++;
		}

		return true;
	}

}

namespace vce
{
	template<typename T>
	struct AutoVariableArrayBuf
	{
		T *var;
		const int n;
		explicit AutoVariableArrayBuf(unsigned char *&p,int N)
			:n(N)
		{
			var=(T*)p;
			for(int i=0;i<n;i++)
			{
				new(p)T;
				p+=sizeof(T);
			}
		}
		~AutoVariableArrayBuf()
		{
			for(int i=0;i<n;i++)
				var[i].~T();
		}
		operator T*()
		{
			return var;
		}
	private:
		void operator =(const AutoVariableArrayBuf&){};
	};

	template<typename T,int N>
	struct AutoArrayBuf
	{
		T *var;
		explicit AutoArrayBuf(unsigned char *&p)
		{
			var=(T*)p;
			for(int i=0;i<N;i++)
			{
				new(p)T;
				p+=sizeof(T);
			}
		}
		~AutoArrayBuf()
		{
			for(int i=0;i<N;i++)
				var[i].~T();
		}
		operator T*()
		{
			return var;
		}
	private:
		void operator =(const AutoArrayBuf&){};
	};

	template<typename T>
	struct AutoBuf
	{
		T &var;
		explicit AutoBuf(unsigned char *&p)
			:var(*new(p)T)
		{
			p+=sizeof(T);
		}
		~AutoBuf()
		{
			var.T::~T();
		}
		operator T&()
		{
			return var;
		}
	private:
		void operator =(const AutoBuf&){};
	};

}

#endif//VCE_VCE2SERIALIZE_H

