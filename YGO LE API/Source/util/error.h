#pragma once

import error;

#define Error(TYPE, ...) Error(TYPE, __FUNCSIG__, __VA_ARGS__)

#define CriticalError(...) Error(Error::Type::CRITICAL_ERROR, __VA_ARGS__)

#define Exception(...) Error(Error::Type::EXCEPTION, __VA_ARGS__)

#define MajorError(...) Error(Error::Type::MAJOR_ERROR, __VA_ARGS__)

#define MinorError(...) Error(Error::Type::MINOR_ERROR, __VA_ARGS__)

#define Success(...) Error(Error::Type::SUCCESS, __VA_ARGS__)

#define Warning(...) Error(Error::Type::WARNING, __VA_ARGS__)
