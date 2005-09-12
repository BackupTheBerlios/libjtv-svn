/*
    Software Security System
    Copyright (C) 2002 SCSS SPECTR  All Rights Reserved

    Security system basic object
*/

#include "cs/cbase.h"

ctBase::~ctBase ()
{
}

void ctBase::DecRef ()
{
  if (!--RefCount)
    delete this;
}
