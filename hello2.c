#include <acknex.h>
#include <default.c>

#define WALK_SPEED        20
#define SLOW_WALK_SPEED   WALK_SPEED/2.0
#define MOUSE_SENSITIVITY 10
#define ACCELERATION      2.0
#define MAX_ACCELERATION  6.0
#define MAX_ROLL_DEG      2.0
#define ROLL_DEG_INC      0.5

// Define the cube entity
ENTITY* cube;

float g_Velocity     = 0.0;
float g_VelocitySide = 0.0;
int   g_ForwardDir   = 0;
float g_RollDeg      = 0.0;

function MoveCamera() {
	VECTOR* newAngle = NULL;
	VECTOR* newPos   = NULL;

//	while(1) {
		// Mouselook
		float angleZ = 0;
		float angleY = 0;		
		angleZ = (camera.pan - mouse_force.x*time_step*MOUSE_SENSITIVITY)%360;
		angleY = (camera.tilt + mouse_force.y*time_step*MOUSE_SENSITIVITY);
		angleY = clamp(angleY, -90, 90);
		newAngle = vector(angleZ, angleY, 0);
		vec_set(camera.pan, newAngle);	

		// Calculate the forward vector. Needed for walking forward/backward and staving
		
		VECTOR* forward = vector(
			cosv(camera.pan), 
			sinv(camera.pan), 
			0);
		vec_normalize(forward, 1.0);		
	
		// Walk forward/backwards
		
		int forwardDir = 0;
		if (key_w) {			
			g_Velocity += time_step*ACCELERATION*ACCELERATION;					
		}
		if (key_s) {
			g_Velocity -= time_step*ACCELERATION*ACCELERATION;			
		}
		if ( (!key_s && !key_w) || (key_s && key_w)) {
			if (g_Velocity > 0.0) {
				g_Velocity -= time_step*ACCELERATION;
			}
			if (g_Velocity < 0.0) {
				g_Velocity += time_step*ACCELERATION;
			}			
		}		
			
		float walkSpeed = WALK_SPEED;
		if (key_shift) {
			walkSpeed = SLOW_WALK_SPEED;
		}			
		g_Velocity = clamp(g_Velocity, -MAX_ACCELERATION, MAX_ACCELERATION);
		float xCmp = time_step*g_Velocity*walkSpeed*forward.x;
		float yCmp = time_step*g_Velocity*walkSpeed*forward.y;
		newPos = vector(
			xCmp,
			yCmp,
			0
		);		
				
		// Straving			
		
		VECTOR right;
		VECTOR* up = vector(0, 0, 1);			
		vec_cross(&right, up, forward);
		if (key_a) {
			g_VelocitySide += time_step*ACCELERATION*ACCELERATION;
			g_RollDeg      -= time_step*ROLL_DEG_INC;
		}
		if (key_d) {
			g_VelocitySide -= time_step*ACCELERATION*ACCELERATION;	
			g_RollDeg      += time_step*ROLL_DEG_INC;
		}
		if ( (!key_a && !key_d) || (key_a && key_d) ) {
			if (g_VelocitySide > 0.0) {
				g_VelocitySide -= time_step*ACCELERATION;
			}
			if (g_RollDeg > 0.0) {
				g_RollDeg -= time_step*ROLL_DEG_INC;
			}
			if (g_VelocitySide < 0.0) {
				g_VelocitySide += time_step*ACCELERATION;
			}			
			if (g_RollDeg < 0.0) {
				g_RollDeg += time_step*ROLL_DEG_INC;
			}
		}
		g_VelocitySide = clamp(g_VelocitySide, -MAX_ACCELERATION, MAX_ACCELERATION);
		if ( abs(g_VelocitySide) < 0.00001 ) {
			g_VelocitySide = 0;
		}
		g_RollDeg = clamp(g_RollDeg, -MAX_ROLL_DEG, MAX_ROLL_DEG);
		vec_scale(right, time_step*g_VelocitySide*walkSpeed);

		vec_add(newPos, right);
		float walkDistance = vec_length(newPos);
		if (walkDistance > 0.00001) {
			vec_add(camera.x, newPos);
		}
		
		camera.roll = g_RollDeg;
		//vec_set(camera.x, newPos);
		
				
//		wait(1);
//	}
}

action RotateEntity() {
	while (1) {
		// Rotate the cube around the Y-axis
	    my.pan += 10 * time_step; // Adjust the rotation speed as needed
	    my.tilt += 5 * time_step;
	    
	    wait(-0.005);
    }
}

// Initialize the game
void main()
{
    video_mode = 12; // Set the video mode (adjust as needed)
    
    level_load("simple_map.wmb"); // Load an empty level

	//camera->flags |= SHOW;
	camera->x = -1500;
	camera->z = 150;	
	    
    // Create the cube
    VECTOR* pos = vector(-100, 0, 150);
    cube = ent_create("blob.mdl", pos, RotateEntity);
    cube.scale_x = 5.0;
    cube.scale_y = 5.0;
    cube.scale_z = 5.0;
    
    while(1)
    {    	    	  
    	if (key_cuu) {
    		cube.z += 30*time_step;
    	}
    	if (key_cud) {
    		cube.z -= 30*time_step;
    	}
    	if (key_cur) {
    		cube.y += 30*time_step;
    	}
    	if (key_cul) {
    		cube.y -= 30*time_step;
    	}
    	
    	if (key_brackr) {
    		cube.scale_x += 1.1*time_step;
    		cube.scale_y += 1.1*time_step;
    		cube.scale_z += 1.1*time_step;
    	}
    	if (key_brackl) {
    		cube.scale_x -= 1.1*time_step;
    		cube.scale_y -= 1.1*time_step;
    		cube.scale_z -= 1.1*time_step;
    	}
    	
    	MoveCamera();
    	  
        // Update the game engine
        wait(1);
    }
}





