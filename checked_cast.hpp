/***************************************************************************** 
  *  AUTHOR:      Toni Schornboeck <toni.schornboeck@gmail.com> 
  *  (c) 2012 by Toni Schornboeck 
  *          
  *  This library is free software; you can redistribute it and/or 
  *  modify it under the terms of the GNU Lesser General Public 
  *  License as published by the Free Software Foundation; either 
  *  version 2.1 of the License, or (at your option) any later version. 
  * 
  *  This library is distributed in the hope that it will be useful, 
  *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
  *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
  *  Lesser General Public License for more details. 
  * 
  *  You should have received a copy of the GNU Lesser General Public 
  *  License along with this library; See the file COPYING. If not, write 
  *  to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, 
  *  Boston, MA 02111-1307 USA 
  *****************************************************************************/ 

#ifndef schorny_CHECKED_CAST_HPP_INCLUDED
#define schorny_CHECKED_CAST_HPP_INCLUDED

#include<exception> 
#include<typeinfo> 
#include<cassert> 
  
//define to force dynamic cast
//#define CHECKED_CAST_SAFE_CONVERSATION 
   
//define to assert on badcast instead of throw bad_cast exception
//#define CHECKED_CAST_DO_ASSERT 
   
namespace hidden 
{ 
//workaroud for T& equally matching f(T&) and f(T const&)
//so we do the following: T const& never matches f(T&), therefore:
//f(T&, LookUpHelper2 const&) and f(T const&, LookUpHelper const&)
//f(T&, LookUpHelper2 const&) does match both but f(T&, LookUpHelper2 const&) is a
//perfect match (no upcast for LookUpHelper needed)

struct LookUpHelper {}; 
struct LookUpHelper2 : public hidden::LookUpHelper {}; 
   
//IsPtr - partial specialization only 
template<typename T> struct IsPtr { enum { value = false }; }; 
template<typename T> struct IsPtr<T*> { enum { value = true }; }; 
} 
   
//bad_checked_cast is thrown if cast is bad
//see boost::lexical_cast
class bad_checked_cast : std::bad_cast 
{ 
private: 
   std::type_info const* from; 
   std::type_info const* to; 
   
public: 
   bad_checked_cast() : from(0), to(0) {} 
   
   bad_checked_cast(std::type_info const& from, std::type_info const& to) 
   : from(&from), to(&to) 
   {} 
   
   std::type_info const& source_type() const 
   { 
     assert(from); 
     return *from; 
   } 
   
   std::type_info const& target_type() const 
   { 
     assert(to); 
     return *to; 
   } 
   
   char const* what() const throw() 
   { 
     return "bad checked cast: source is not a target type"; 
   } 
}; 
#ifdef CHECKED_CAST_DO_ASSERT 
  #define BAD_CHECKED_CAST(from, to) assert(false) 
#else 
  #define BAD_CHECKED_CAST(from, to) throw ::bad_checked_cast(typeid(from), typeid(to)) 
#endif 
   
   
//implementation
namespace hidden 
{ 
template<typename T, typename X, bool isPtr> 
struct checked_cast_impl; 
   
//pointer variant 
template<typename T, typename X> 
struct checked_cast_impl<T, X, true> 
{ 
   static T cast(X& x, hidden::LookUpHelper2 const&) 
   { 
#ifdef CHECKED_CAST_SAFE_CONVERSATION 
     T t = dynamic_cast<T>(x); 
   
     //check cross cast
     if (t!=static_cast<T>(x)) BAD_CHECKED_CAST(x, T); 
     return t; 
#else 
     return static_cast<T>(x); 
#endif 
   } 
   
   static T cast(X const& x, hidden::LookUpHelper const&) 
   { 
#ifdef CHECKED_CAST_SAFE_CONVERSATION 
     T t = dynamic_cast<T>(x); 
   
     //check cross cast
     if (t!=static_cast<T>(x)) BAD_CHECKED_CAST(x, T); 
     return t; 
#else 
     return static_cast<T>(x); 
#endif 
   } 
}; 
   
template<typename T, typename X> 
struct checked_cast_impl<T, X, false> 
{ 
   static T cast(X& x, hidden::LookUpHelper2 const&) 
   { 
#ifdef CHECKED_CAST_SAFE_CONVERSATION 
     try 
     { 
       T t=dynamic_cast<T>(x); 
       
       //check cross cast
       if(&t!=&static_cast<T>(x)) throw std::bad_cast(); 
       return t; 
     } 
     catch(...) 
     { 
       BAD_CHECKED_CAST(x, T); 
     } 
#else 
     return static_cast<T>(x); 
#endif 
   } 
   
   static T cast(X const& x, hidden::LookUpHelper const&) 
   { 
#ifdef CHECKED_CAST_SAFE_CONVERSATION 
     try 
     { 
       T t=dynamic_cast<T>(x); 
       
       //check cross cast
       if(&t!=&static_cast<T>(x)) std::bad_cast(); 
       return t; 
     } 
     catch(...) 
     { 
       BAD_CHECKED_CAST(x, T); 
     } 
#else 
     return static_cast<T>(x); 
#endif 
   } 
}; 
   
} 
   
template<typename T, typename X> 
inline T checked_cast(X& x) 
{ 
   return hidden::checked_cast_impl<T, X, hidden::IsPtr<X>::value>::cast(x, hidden::LookUpHelper2()); 
} 
template<typename T, typename X> 
inline T checked_cast(X const& x) 
{ 
   return hidden::checked_cast_impl<T, X, hidden::IsPtr<X>::value>::cast(x, hidden::LookUpHelper2()); 
} 
   
#endif
