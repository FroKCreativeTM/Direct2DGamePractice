#pragma once

#include "../GameFramework.h"

enum class GAME_ERROR_TYPE
{
	GET_FATAL,
	GET_WARNING
};

class CGameError : 
	public std::exception
{
public : 
	CGameError() throw();
	CGameError(const CGameError& ref) throw();
	CGameError(GAME_ERROR_TYPE eErrorType, std::string strMessage) throw();
	CGameError& operator=(const CGameError& rhs) throw();

	virtual ~CGameError() throw();
	virtual const char* What() const throw();
	const char* GetErrorMessage() const throw();
	GAME_ERROR_TYPE GetErrorCode() const throw();

private : 
	GAME_ERROR_TYPE		m_eErrorCode;
	std::string			m_strMessage;
};

