#include <acknex.h>
#include <default.c>

#define WALK_SPEED 20

// Define the cube entity
ENTITY* cube;

function MoveCamera() {
	VECTOR* newAngle = NULL;
	VECTOR* newPos   = NULL;

//	while(1) {
		float angle = 0;
		int dir = 0;
		if (key_a) {					
			dir += 1;
		}
		if (key_d) {						
			dir += -1;
		}
		angle = (camera.pan + dir*time_step*WALK_SPEED)%360;
		newAngle = vector(angle, 0, 0);
		vec_set(camera.pan, newAngle);	

		int forwardDir = 0;
		if (key_w) {
			forwardDir += 1;
		}
		if (key_s) {
			forwardDir -= 1;
		}
		float xCmp = camera.x + forwardDir*cosv(camera.pan)*time_step*WALK_SPEED;
		float yCmp = camera.y + forwardDir*sinv(camera.pan)*time_step*WALK_SPEED;
		newPos = vector(
			xCmp,
			yCmp,
			0
		);
		
		vec_set(camera.x, newPos);
		
		
		vec_normalize(newPos, 1.0);
		VECTOR right;
		VECTOR* up = vector(0, 0, 1);
		vec_cross(&right, up, newPos);
		if (key_u) {
			vec_scale(right, time_step*WALK_SPEED);
			vec_add(camera.x, right);
//			vec_set(camera.x, right);
		}
		if (key_y) {
			vec_scale(right, -1.0*time_step*WALK_SPEED);
			vec_add(camera.x, right);
		}
		

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
	camera->z = 100;	
	    
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





