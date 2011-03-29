/*
  VERSION 0.2, 16 november 2003

  Copyright (c) 2003 Bart Samwel
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:
  1. Redistributions of source code must retain the above copyright
  notice, this list of conditions and the following disclaimer.
  2. The name of the author may not be used to endorse or promote products
  derived from this software without specific prior written permission.
*/

#ifndef ENSURE_OVERRIDE_H
#define ENSURE_OVERRIDE_H

/*
  Use this macro to make sure that a member function in a derived class
  actually matches a member function in a specific base class. This is
  especially useful when you don't call the base class version from the
  overriding function. However, it also comes in useful in other cases.
  For instance, if you suddenly decide to add a parameter with a default
  value to the base class version, it's easy to forget to add it to the
  versions in the derived classes, and even if the derived class versions
  call the base class versions *they still compile*. When you use this
  macro to define your member function overrides, you will not run into
  this specific kind of trouble anymore.(The reason I wrote this macro
  is that I ran into exactly this kind of trouble a couple of days ago,
  and I thought I should do something to prevent this from happening to
  me ever again. :) )

  It works like this. Instead of writing (in class B derived from A):

  void foo(int,double);
 
  you now write 

  OVERRIDE(A,void,foo,(int,double))
  {
    // ...
  }

  Now, when the signature of foo in A changes so that A::foo(int,double)
  does not exist anymore, you will get an error stating this. Also, if B
  suddenly isn't derived from A anymore, you will get an error as well.
*/

// Define WITHOUT_OVERRIDE_CHECKING if you want to disable the checks.
// The checks do generate code, and you might want to get rid of that
// in release builds.
#ifdef WITHOUT_OVERRIDE_CHECKING

#define OVERRIDE(baseclass,returntype,func,_lb_params_rb_)	\
  returntype func _lb_params_rb_

#else


#define OVERRIDE(baseclass,returntype,func,_lb_params_rb_) \
  void func##_override_test_function _lb_params_rb_	\
  {	\
    returntype (baseclass::*fntestptr) _lb_params_rb_ = &baseclass::func; \
    const baseclass *basetestptr = this; \
    (void)fntestptr; \
    (void)basetestptr; \
  }; \
  returntype func _lb_params_rb_

#endif

/*
 * Define TEST_OVERRIDE to test the macro.
 */
#ifdef TEST_OVERRIDE
class A
{
protected:
  void foo(int);
  void foo();
};

class B : public A
{
public:
  // This should be OK.
  OVERRIDE(A,void,foo,(int));

  // This should give an error, because A::foo(int,double) does not exist.
  OVERRIDE(A,void,foo,(int,double));
};

void A::foo(int)
{
}


void B::foo(int,double)
{
}

int main()
{
}

#endif

#endif // OVERRIDE_H
