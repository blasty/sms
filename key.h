#ifndef __KEY_H__
#define __KEY_H__

void key_pressed( unsigned char key, int x, int y );
void key_released( unsigned char key, int x, int y );
void key_special_pressed( int key, int x, int y );
void key_special_released( int key, int x, int y );

#endif
