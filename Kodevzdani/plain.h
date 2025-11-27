/**
 * @file plain.h
 *
 * @brief Plain model 
 *
 * @details  Command for rendering
 *           glDrawArrays(GL_TRIANGLES, 0, 6);
 *
 * @author Martin Nemec
 *
 * @year 2023
 **/


const float plane[] = {
    // pos                normal       uv
    -5, 0, -5,            0,1,0,       0,  0,
     5, 0, -5,            0,1,0,       5, 0,
     5, 0,  5,            0,1,0,       5,5,

    -5, 0, -5,            0,1,0,       0,  0,
     5, 0,  5,            0,1,0,       5,5,
    -5, 0,  5,            0,1,0,       0, 5
};
