#version 430 core

layout(points) in;
layout(points, max_vertices=2) out;

uniform mat4 projMat;
uniform mat4 modelViewMat;

in vec4 coords[];
in vec3 vel[];

out vec4 updatedCoords;
out vec3 updatedVel;

void main(void)
{ 
   // If the particle crosses the right boundary, destroy the particle, creating two new ones
   // from it whose coordinates are set to the previous values of the destroyed particle's
   // coordinates to keep them inside the square; new velocities are set as well so that
   // the new particles move into the square, one making 45 degrees with the right side,
   // the other tan^{-1} 0.5 = 26.57 degrees.
   if (coords[0].x > 95.0)
   {	  
      updatedCoords = coords[0] - vec4(vel[0], 0.0);
	  updatedVel = vec3(-1.0, 1.0, 0.0);
      gl_Position = projMat * modelViewMat * updatedCoords;
      EmitVertex( );
      EndPrimitive( );

      updatedCoords = coords[0] - vec4(vel[0], 0.0);
	  updatedVel = vec3(-1.0, -2.0, 0.0);
      gl_Position = projMat * modelViewMat * updatedCoords;
      EmitVertex( );
      EndPrimitive( );  
   }

   // If the particle crosses the left boundary, ...
   else if (coords[0].x < -95.0)
   {	  
      updatedCoords = coords[0] - vec4(vel[0], 0.0);
	  updatedVel = vec3(1.0, 2.0, 0.0);
      gl_Position = projMat * modelViewMat * updatedCoords;
      EmitVertex( );
      EndPrimitive( );

      updatedCoords = coords[0] - vec4(vel[0], 0.0);
	  updatedVel = vec3(1.0, -1.0, 0.0);
      gl_Position = projMat * modelViewMat * updatedCoords;
      EmitVertex( );
      EndPrimitive( );  
   }

   // If the particle crosses the top boundary, ...
   else if (coords[0].y > 95.0)
   {	  
      updatedCoords = coords[0] - vec4(vel[0], 0.0);
	  updatedVel = vec3(2.0, -1.0, 0.0);
      gl_Position = projMat * modelViewMat * updatedCoords;
      EmitVertex( );
      EndPrimitive( );

      updatedCoords = coords[0] - vec4(vel[0], 0.0);
	  updatedVel = vec3(-1.0, -1.0, 0.0);
      gl_Position = projMat * modelViewMat * updatedCoords;
      EmitVertex( );
      EndPrimitive( );  
   }

   // If the particle crosses the bottom boundary, ...
   else if (coords[0].y < -95.0)
   {	  
      updatedCoords = coords[0] - vec4(vel[0], 0.0);
	  updatedVel = vec3(2.0, 1.0, 0.0);
      gl_Position = projMat * modelViewMat * updatedCoords;
      EmitVertex( );
      EndPrimitive( );

      updatedCoords = coords[0] - vec4(vel[0], 0.0);
	  updatedVel = vec3(-1.0, 1.0, 0.0);
      gl_Position = projMat * modelViewMat * updatedCoords;
      EmitVertex( );
      EndPrimitive( );  
   }

   // If the particle is within the square then draw it with its current
   // coordinates and write the updated coordinates via transform feedback.
   else
   {	  
      updatedCoords = coords[0] + vec4(vel[0], 0.0);
	  updatedVel = vel[0];
      gl_Position = projMat * modelViewMat * coords[0];
      EmitVertex( );
      EndPrimitive( );
   }
}   