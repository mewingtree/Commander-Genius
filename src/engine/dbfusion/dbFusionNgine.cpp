#include "dbFusionNgine.h"
#include "engine/CGameLauncher.h"

#include "menu/MainMenu.h"

#include <base/video/CVideoDriver.h>
#include <base/utils/FindFile.h>

#include <base/CInput.h>
#include <widgets/GsMenuController.h>

int dosbox_main(int argc, const char* argv[]);


bool dosMachinePause;

extern bool dosMapperRunning;

namespace dbfusion
{

std::string globGamePath;

const unsigned int maxStrLen = 256;

int mainDosbox(void*)
{
    int argc = 1;

    char **argv;

    if(!globGamePath.empty())
    {
        argc = 2;
    }

    // Allocate the more or less primitive way
    argv = new char* [2];
    for(int i=0 ; i<argc ; i++)
    {
        argv[i] = new char[maxStrLen];
    }

    strcpy(argv[0], "dosbox");

    if(!globGamePath.empty())
    {
        strcpy(argv[1], globGamePath.c_str());
    }

    dosbox_main(argc, (const char**) argv);

    // Deallocate the more or less primitive way
    for(int i=0 ; i<argc ; i++)
    {
        delete [] argv[i];
    }
    delete [] argv;


    return 0;
}

void DBFusionEngine::start()
{
    const GsRect<Uint16> dosRect(640, 400);
    gVideoDriver.setNativeResolution(dosRect);


    globGamePath = GetAbsolutePath(mGamePath);

    mp_Thread.reset(threadPool->start(mainDosbox, nullptr, "DosBoxMain"));
}

void DBFusionEngine::pumpEvent(const CEvent *evPtr)
{
    if( dynamic_cast<const BackButtonSendDosFusion*>(evPtr) )
    {
        gInput.pushBackButtonEventExtEng();
        gEventManager.add(new CloseMenuEvent());
    }

    if( const ExecuteMappperEvent *eme = dynamic_cast<const ExecuteMappperEvent*>(evPtr) )
    {
        (*eme)();
        gEventManager.add(new CloseMenuEvent());
    }

}

void DBFusionEngine::ponder(const float deltaT)
{
    int status;

    if( gMenuController.empty() && dosMachinePause )
    {
        dosMachinePause = false;

        GsWeakSurface blit(gVideoDriver.getBlitSurface());
        mBackbuffer.blitTo(blit);
    }

    if(threadPool->finalizeIfReady(mp_Thread.get(), &status))
    {
        gEventManager.add(new GMSwitchToGameLauncher);

        mp_Thread.release();
        return;
    }

    if(!mMenuLocked && !dosMapperRunning)
    {
        // Did the player press the quit/back button
        if( gInput.getPressedCommand(IC_BACK) )
        {
            if( gMenuController.empty() ) // If no menu is open, open the main menu
            {
                gEventManager.add( new OpenMenuEvent( new MainMenu() ) );
                dosMachinePause = true;

                GsWeakSurface blit(gVideoDriver.getBlitSurface());
                mBackbuffer.createCopy(blit);
            }
        }
    }

}

void DBFusionEngine::render()
{

}


}
