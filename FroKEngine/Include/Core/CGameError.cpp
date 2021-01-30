#include "CGameError.h"

CGameError::CGameError() throw() :
	m_eErrorCode(GAME_ERROR_TYPE::GET_FATAL),
	m_strMessage("Undefined Error in game.") {}

CGameError::CGameError(const CGameError& ref) throw() :
	m_eErrorCode(ref.m_eErrorCode),
	m_strMessage(ref.m_strMessage) {}

CGameError::CGameError(GAME_ERROR_TYPE eErrorType, std::string strMessage) throw()
	: m_eErrorCode(eErrorType), m_strMessage(strMessage) {}

CGameError& CGameError::operator=(const CGameError& rhs) throw()
{
	std::exception::operator=(rhs);
	this->m_eErrorCode = rhs.m_eErrorCode;
	this->m_strMessage = rhs.m_strMessage;

	return *this;
}

CGameError::~CGameError() throw() {}

const char* CGameError::What() const throw()
{
	return this->GetErrorMessage();
}

const char* CGameError::GetErrorMessage() const throw()
{
	return m_strMessage.c_str();
}

GAME_ERROR_TYPE CGameError::GetErrorCode() const throw()
{
	return m_eErrorCode;
}
