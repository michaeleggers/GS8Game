#include <acknex.h>
#include <default.c>

#define WALK_SPEED        1.0
#define SLOW_WALK_SPEED   WALK_SPEED/2.0
#define MOUSE_SENSITIVITY 10
#define ACCELERATION      2.0
#define MAX_ACCELERATION  6.0
#define MAX_ROLL_DEG      2.0
#define ROLL_DEG_INC      0.5
#define CAM_WAVE_INC      0.5
#define CAM_WAVE_STRENGTH 5.0

// Define the cube entity
ENTITY* cube;

float g_Velocity     = 0.0;
float g_VelocitySide = 0.0;
int   g_ForwardDir   = 0;
float g_RollDeg      = 0.0;
float g_CamWave      = 0.0;
float g_SaveCamZ     = 150.0;

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
			forwardDir  = 1;		
			g_Velocity += time_step*ACCELERATION*ACCELERATION;
			g_CamWave  += time_step*CAM_WAVE_INC;
		}
		if (key_s) {
			forwardDir  = -1;
			g_Velocity -= time_step*ACCELERATION*ACCELERATION;			
			g_CamWave  += time_step*CAM_WAVE_INC;
		}
		if ( (!key_s && !key_w) || (key_s && key_w)) {
			if (g_Velocity > 0.0) {
				g_Velocity -= time_step*ACCELERATION;
			}
			if (g_Velocity < 0.0) {
				g_Velocity += time_step*ACCELERATION;
			}
		
			// Check where we are at the sine urve. If still > 0 then keep doing until done
			float sineOfCurrent = sin(g_CamWave);
			if (sineOfCurrent > 0.0) {
				g_CamWave += time_step*CAM_WAVE_INC;
			}
			else { // Done with waving -> Reset the input value so we start at sin(0) = 0 again.
				g_CamWave = 0.0;
			}
		}
		//g_CamWave = clamp(g_CamWave, 0.0, 360.0);
	   
			
		float walkSpeed = WALK_SPEED;
		if (key_shift) {
			walkSpeed = SLOW_WALK_SPEED;
		}			
		g_Velocity = clamp(g_Velocity, -MAX_ACCELERATION, MAX_ACCELERATION);
		if (abs(g_Velocity) < 0.001) {
			g_Velocity = 0;
		}
		float xCmp = forward.x;
		float yCmp = forward.y;
		newPos = vector(
			xCmp,
			yCmp,
			0
		);		

				
		// Straving			

		
		int sideDir = 0;
		VECTOR right;
		VECTOR* up = vector(0, 0, 1);			
		vec_cross(&right, up, forward);
		if (key_a) {
			g_VelocitySide += time_step*ACCELERATION*ACCELERATION;						
			sideDir        += 1;
			g_RollDeg      -= time_step*2*ROLL_DEG_INC;
		}
		if (key_d) {
			g_VelocitySide += time_step*ACCELERATION*ACCELERATION;							
			sideDir        -= 1;
			g_RollDeg      += time_step*2*ROLL_DEG_INC;
		}
		if ( (!key_a && !key_d) || (key_a && key_d) ) {
			if (g_VelocitySide > 0.0) {
				g_VelocitySide -= time_step*ACCELERATION;
			}
			if (g_RollDeg > 0.0) {
				g_RollDeg -= time_step*ROLL_DEG_INC;
			}
//			if (g_VelocitySide < 0.0) {
//				g_VelocitySide += time_step*ACCELERATION;
//			}			
			if (g_RollDeg < 0.0) {
				g_RollDeg += time_step*ROLL_DEG_INC;
			}
		}
		g_VelocitySide = clamp(g_VelocitySide, 0, MAX_ACCELERATION);
		if ( abs(g_VelocitySide) < 0.001 ) {
			g_VelocitySide = 0;
		}
		
		vec_scale(right, g_VelocitySide*sideDir);
		vec_scale(newPos, g_Velocity);
		vec_add(newPos, right);
		vec_normalize(newPos, walkSpeed);
		//vec_scale(newPos, walkSpeed);
		vec_add(camera.x, newPos);
		float sinAt = abs(sin(g_CamWave));
		//sinAt = smooth(&sinAt, 0.09);
		camera.z = g_SaveCamZ + CAM_WAVE_STRENGTH*sinAt;		
		g_RollDeg = clamp(g_RollDeg, -MAX_ROLL_DEG, MAX_ROLL_DEG);
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
	camera->z = g_SaveCamZ;	
	    
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





