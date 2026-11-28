#include "Game.h"
#include <GLFW/glfw3.h>

void MiningGame::initialize(InputMappingManager& inputMappings)
{
    // PLAYER 1
    InputMappingContext player1;
    player1.name = "Player1";
    player1.enabled = false;
    player1.priority = 0;
	
    player1.bindAxis2DKey("Move", GLFW_KEY_W, {0, 1});
    player1.bindAxis2DKey("Move", GLFW_KEY_S, {0, -1});
    player1.bindAxis2DKey("Move", GLFW_KEY_A, {-1, 0});
    player1.bindAxis2DKey("Move", GLFW_KEY_D, {1, 0});
	
    player1.bindMouseMotion("Look");
	
    player1.bindAxis1DKey("Vertical", GLFW_KEY_SPACE, 1.0f);
    player1.bindAxis1DKey("Vertical", GLFW_KEY_LEFT_CONTROL, -1.0f);
	
	inputMappings.addContext(player1);
	
    //Player 2
    InputMappingContext player2;
    player2.name = "Player2";
    player2.enabled = false;
    player2.priority = 0;

    player2.bindAxis2DKey("Move", GLFW_KEY_UP, {0, 1});
    player2.bindAxis2DKey("Move", GLFW_KEY_DOWN, {0, -1});
    player2.bindAxis2DKey("Move", GLFW_KEY_LEFT, {-1, 0});
    player2.bindAxis2DKey("Move", GLFW_KEY_RIGHT, {1, 0});
	
    player2.bindAxis2DKey("KeyboardLook", GLFW_KEY_KP_8, {0, -1});
    player2.bindAxis2DKey("KeyboardLook", GLFW_KEY_KP_2, {0, 1});
    player2.bindAxis2DKey("KeyboardLook", GLFW_KEY_KP_4, {-1, 0});
    player2.bindAxis2DKey("KeyboardLook", GLFW_KEY_KP_6, {1, 0});

    player2.bindAxis1DKey("Vertical", GLFW_KEY_RIGHT_SHIFT, 1.0f);
    player2.bindAxis1DKey("Vertical", GLFW_KEY_RIGHT_CONTROL, -1.0f);

	inputMappings.addContext(player2);
}