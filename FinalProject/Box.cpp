#include "Box.hpp"
#include <iostream>

Box::Box(int directions,float x,float y,float w,float h){
    
    
        
        if(directions==0){
            //std::cout<<"I'm IN!\n";
            
            picture[0] = al_load_bitmap("resources/images/play/box00.png");
            picture[1] = al_load_bitmap("resources/images/play/box01.png");
        }
        else if(directions==1){
            picture[0] = al_load_bitmap("resources/images/play/box10.png");
            picture[1] = al_load_bitmap("resources/images/play/box11.png");
        }
        else if(directions==2){
            picture[0] = al_load_bitmap("resources/images/play/box20.png");
            picture[1] = al_load_bitmap("resources/images/play/box21.png");
        }
        else if(directions==3){
            
            picture[0] = al_load_bitmap("resources/images/play/box30.png");
            picture[1] = al_load_bitmap("resources/images/play/box31.png");
        }
    
    
    
    Position.x = x;
    Position.y = y;
    Size.x = w;
    Size.y = h;
    state = 0;
    
}

void Box::Draw()const{
    
    
    al_draw_bitmap(picture[state  ], Position.x, Position.y, 1);
    
    
}
