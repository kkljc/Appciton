#include "PublicMenuManager.h"
#include "WindowManager.h"

PublicMenuManager *PublicMenuManager::_selfObj = NULL;

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
PublicMenuManager::PublicMenuManager() : MenuGroup("PublicMenuManager")
{
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
PublicMenuManager::~PublicMenuManager()
{

}