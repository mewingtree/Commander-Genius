/*
 * GameEngine.h
 *
 *  Created on: 29.03.2012
 *      Author: gerstrong
 */

#include "mode/CGameMode.h"
#include "CResourceLoader.h"

#include <base/GsEngine.h>
#include <memory>

#ifndef GAMEENGINE_H
#define GAMEENGINE_H

class GameEngine : public GsEngine
{
public:
    GameEngine(const bool openedGamePlay,
               const std::string &datapath) :
        mOpenedGamePlay(openedGamePlay),
        mDataPath(datapath)
    {}

    virtual void start() = 0;

    void ponder(const float deltaT);

    void render();

    void renderFPSDisplay();

    virtual void pumpEvent(const CEvent *evPtr);


protected:

    CResourceLoaderBackground mEngineLoader;	

    std::unique_ptr<CGameMode> mpGameMode;

    bool mOpenedGamePlay;

    const std::string mDataPath;

private:

    std::shared_ptr<SDL_Surface> mpFPSSurface;

};


#endif /* GAMEENGINE_H */
