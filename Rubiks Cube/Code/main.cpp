#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader.hpp"
#include <vector>
#include <algorithm>
#include <string>
#include <ctime>
#include <iostream>
#include <fstream>
#include <cmath>
#include <map>

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

// camera
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 10.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -10.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

// Rotation vectors
glm::vec3 rot_Z_right = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 rot_Z_left = glm::vec3(0.0f, 0.0f, 1.0f);
glm::vec3 rot_Y_right = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 rot_Y_left = glm::vec3(0.0f, -1.0f, 0.0f);
glm::vec3 rot_X_up = glm::vec3(-1.0f, 0.0f, 0.0f);
glm::vec3 rot_X_down = glm::vec3(1.0f, 0.0f, 0.0f);

vector<vector<int>> cube_colors;
vector<vector<int>> sorted_cube_colors(6);
vector<vector<int>> faces(6);
vector<int> current_cube_and_face;

// settings
int SCR_WIDTH = 800;
int SCR_HEIGHT = 600;
int pane_num;
int rot_direction;
float rotation_rate = 0.02f;
bool init = false;
int init_count;
bool got_swipe = false;

bool firstMouse = true;
float yaw = 90.0f;
float pitch = 0.0f;
float lastX = 0;
float lastY = 0;
float initial_x = 0;
float initial_y = 0;
float fov = 45.0f;
int color_ID = 0;


// Flatten 2D vector to 1D
vector<int> flatten(vector<vector<int>> pane) {

    vector<int> flat_vec;

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            flat_vec.push_back(pane[i][j]);
        }
    }
    return flat_vec;
}

// When rotation is made, update rotated cubes face color ID's
void update_cube_colors(vector<int> cubes, glm::vec3 rot_axis) {

    vector<vector<int>> copy_cube_colors = cube_colors;

    for (int i = 0; i < 9; i++) {
        // rot Z right
        if (rot_axis == glm::vec3(0.0f, 0.0f, -1.0f)) {
            cube_colors[cubes[i]][0] = copy_cube_colors[cubes[i]][0];
            cube_colors[cubes[i]][1] = copy_cube_colors[cubes[i]][1];
            cube_colors[cubes[i]][2] = copy_cube_colors[cubes[i]][4];
            cube_colors[cubes[i]][3] = copy_cube_colors[cubes[i]][5];
            cube_colors[cubes[i]][4] = copy_cube_colors[cubes[i]][3];
            cube_colors[cubes[i]][5] = copy_cube_colors[cubes[i]][2];
        }
        // rot Z left
        else if (rot_axis == glm::vec3(0.0f, 0.0f, 1.0f)) {
            cube_colors[cubes[i]][0] = copy_cube_colors[cubes[i]][0];
            cube_colors[cubes[i]][1] = copy_cube_colors[cubes[i]][1];
            cube_colors[cubes[i]][2] = copy_cube_colors[cubes[i]][5];
            cube_colors[cubes[i]][3] = copy_cube_colors[cubes[i]][4];
            cube_colors[cubes[i]][4] = copy_cube_colors[cubes[i]][2];
            cube_colors[cubes[i]][5] = copy_cube_colors[cubes[i]][3];
        }
        // rot Y right
        else if (rot_axis == glm::vec3(0.0f, 1.0f, 0.0f)) {
            cube_colors[cubes[i]][0] = copy_cube_colors[cubes[i]][3];
            cube_colors[cubes[i]][1] = copy_cube_colors[cubes[i]][2];
            cube_colors[cubes[i]][2] = copy_cube_colors[cubes[i]][0];
            cube_colors[cubes[i]][3] = copy_cube_colors[cubes[i]][1];
            cube_colors[cubes[i]][4] = copy_cube_colors[cubes[i]][4];
            cube_colors[cubes[i]][5] = copy_cube_colors[cubes[i]][5];
        }
        // rot Y left
        else if (rot_axis == glm::vec3(0.0f, -1.0f, 0.0f)) {
            cube_colors[cubes[i]][0] = copy_cube_colors[cubes[i]][2];
            cube_colors[cubes[i]][1] = copy_cube_colors[cubes[i]][3];
            cube_colors[cubes[i]][2] = copy_cube_colors[cubes[i]][1];
            cube_colors[cubes[i]][3] = copy_cube_colors[cubes[i]][0];
            cube_colors[cubes[i]][4] = copy_cube_colors[cubes[i]][4];
            cube_colors[cubes[i]][5] = copy_cube_colors[cubes[i]][5];
        }
        // rot X down
        else if (rot_axis == glm::vec3(1.0f, 0.0f, 0.0f)) {
            cube_colors[cubes[i]][0] = copy_cube_colors[cubes[i]][4];
            cube_colors[cubes[i]][1] = copy_cube_colors[cubes[i]][5];
            cube_colors[cubes[i]][2] = copy_cube_colors[cubes[i]][2];
            cube_colors[cubes[i]][3] = copy_cube_colors[cubes[i]][3];
            cube_colors[cubes[i]][4] = copy_cube_colors[cubes[i]][1];
            cube_colors[cubes[i]][5] = copy_cube_colors[cubes[i]][0];
        }
        // rot X up
        else if (rot_axis == glm::vec3(-1.0f, 0.0f, 0.0f)) {
            cube_colors[cubes[i]][0] = copy_cube_colors[cubes[i]][5];
            cube_colors[cubes[i]][1] = copy_cube_colors[cubes[i]][4];
            cube_colors[cubes[i]][2] = copy_cube_colors[cubes[i]][2];
            cube_colors[cubes[i]][3] = copy_cube_colors[cubes[i]][3];
            cube_colors[cubes[i]][4] = copy_cube_colors[cubes[i]][0];
            cube_colors[cubes[i]][5] = copy_cube_colors[cubes[i]][1];
        }
    }
}

// Update what colors are in each face of rubiks cube
void update_rubiks_faces(vector<glm::mat4> models) {

    vector<vector<int>> temp_faces(6);

    for (int i = 0; i < 27; i++) {
        // back Z face
        if ((round(models[i][3][2]) == -1)) {
            temp_faces[0].push_back(cube_colors[i][0]);
        }
        // front Z face
        else if ((round(models[i][3][2]) == 1)) {
            temp_faces[1].push_back(cube_colors[i][1]);
        }

        // left X face
        if ((round(models[i][3][0]) == -1)) {
            temp_faces[2].push_back(cube_colors[i][2]);
        }
        // right X face
        else if ((round(models[i][3][0]) == 1)) {
            temp_faces[3].push_back(cube_colors[i][3]);
        }

        // bottom Y face
        if ((round(models[i][3][1]) == -1)) {
            temp_faces[4].push_back(cube_colors[i][4]);
        }
        // top Y face
        else if ((round(models[i][3][1]) == 1)) {
            temp_faces[5].push_back(cube_colors[i][5]);
        }
    }
    faces = temp_faces;
}


// Check if Rubik's Cube is solved
bool check_complete() {

    int faces_found = 0;

    for (int i = 0; i < 6; i++) {
        bool found = true;
        vector<int> face = faces[i];
        sort(face.begin(), face.end());

        for (int j = 0; j < 6; j++) {
            if (face == sorted_cube_colors[j]) {
                faces_found++;
                break;
            }      
        }
    }
    if (faces_found == 6) {
        return true;
    }
    return false;
}


// Find cube ID and what face of the cube was clicked on from color ID
vector<int> get_cube_ID(int color_ID) {
    vector<int> cube_ID_and_face;

    for (int i = 0; i < 27; i++) {
        for (int j = 0; j < 6; j++) {
            if (cube_colors[i][j] == 0) {
                continue;
            }
            if (color_ID == cube_colors[i][j]){
                cube_ID_and_face.push_back(i);
                cube_ID_and_face.push_back(j);
                return cube_ID_and_face;
            }
        }
    }
}


// Determine what pane should be rotated and what direction based on user swipe
void get_rotation_from_swipe(int start_x, int start_y, int end_x, int end_y, float h_angle, float v_angle, vector<vector<vector< int >>> panes) {
    int z_pane = 10;
    int y_pane = 10;
    int x_pane = 10;
    char swipe_direction;

    // Check which 3 panes the cube clicked on is in. If one of the 3 panes the cube is in is the face that was clicked, dont use it
    for (int i = 0; i < 9; i++) {
        vector<int> temp_pane = flatten(panes[i]);
        if (find(temp_pane.begin(), temp_pane.end(), current_cube_and_face[0]) != temp_pane.end()) {
            switch (i) {
            case 0:
                if (current_cube_and_face[1] != 0) {
                    z_pane = i;
                }
                break;
            case 1:
                z_pane = i;
                break;
            case 2:
                if (current_cube_and_face[1] != 1) {
                    z_pane = i;
                }
                break;
            case 3:
                if (current_cube_and_face[1] != 4) {
                    y_pane = i;
                }
                break;
            case 4:
                y_pane = i;
                break;
            case 5:
                if (current_cube_and_face[1] != 5) {
                    y_pane = i;
                }
                break;
            case 6:
                if (current_cube_and_face[1] != 2) {
                    x_pane = i;
                }
                break;
            case 7:
                x_pane = i;
                break;
            case 8:
                if (current_cube_and_face[1] != 3) {
                    x_pane = i;
                }
                break;
            }
        }
    }

    //(0 = left / down, 1 = right / up)

    // If the swipe is horizontal
    if (abs(start_x - end_x) > (abs(start_y - end_y))) {
        if (start_x > end_x) {
            swipe_direction = 'L'; // Left swipe
        }
        else {
            swipe_direction = 'R'; // Right swipe
        }
    }
    // If the swipe is vertical
    else {
        if (start_y > end_y) {
            swipe_direction = 'D'; // Down swipe
        }
        else {
            swipe_direction = 'U'; // Up swipe
        }
    }

    // Get horizontal angle between 0-360 degrees
    while (h_angle < 0) {
        h_angle += 360;
    }
    h_angle = ((int)round(h_angle)) % 360;


    // Looking at TOP of cube
    if (v_angle > 45) {

        // Looking at TOP FRONT of cube 
        if ((h_angle > 45) && (h_angle < 135)) {
            switch (swipe_direction) {
            case 'L':
                pane_num = z_pane;
                rot_direction = 0;
                break;
            case 'R':
                pane_num = z_pane;
                rot_direction = 1;
                break;
            case 'D':
                pane_num = x_pane;
                rot_direction = 0;
                break;
            case 'U':
                pane_num = x_pane;
                rot_direction = 1;
                break;
            }
        }

        // Looking at TOP LEFT of cube  
        if ((h_angle > 135) && (h_angle < 225)) {
            switch (swipe_direction) {
            case 'L':
                pane_num = x_pane;
                rot_direction = 1;
                break;
            case 'R':
                pane_num = x_pane;
                rot_direction = 0;
                break;
            case 'D':
                pane_num = z_pane;
                rot_direction = 0;
                break;
            case 'U':
                pane_num = z_pane;
                rot_direction = 1;
                break;
            }
        }

        // Looking at TOP BACK of cube
        if ((h_angle > 225) && (h_angle < 315)) {
            switch (swipe_direction) {
            case 'L':
                pane_num = z_pane;
                rot_direction = 1;
                break;
            case 'R':
                pane_num = z_pane;
                rot_direction = 0;
                break;
            case 'D':
                pane_num = x_pane;
                rot_direction = 1;
                break;
            case 'U':
                pane_num = x_pane;
                rot_direction = 0;
                break;
            }
        }

        // Looking at TOP RIGHT of cube
        if ((h_angle > 315) || (h_angle < 45)) {
            switch (swipe_direction) {
            case 'L':
                pane_num = x_pane;
                rot_direction = 0;
                break;
            case 'R':
                pane_num = x_pane;
                rot_direction = 1;
                break;
            case 'D':
                pane_num = z_pane;
                rot_direction = 1;
                break;
            case 'U':
                pane_num = z_pane;
                rot_direction = 0;
                break;
            }
        }
    }

    // Looking at cube at 0 degree vertical angle
    else if ((v_angle > -45) && (v_angle < 45)) {
        if (swipe_direction == 'L') {
            pane_num = y_pane;
            rot_direction = 0;
        }
        else if (swipe_direction == 'R') {
            pane_num = y_pane;
            rot_direction = 1;
        }

        // Looking at FRONT of cube 
        if ((h_angle > 45) && (h_angle < 135)) {
            switch (swipe_direction) {
            case 'D':
                pane_num = x_pane;
                rot_direction = 0;
                break;
            case 'U':
                pane_num = x_pane;
                rot_direction = 1;
                break;
            }
        }

        // Looking at LEFT of cube  
        if ((h_angle > 135) && (h_angle < 225)) {
            switch (swipe_direction) {
            case 'D':
                pane_num = z_pane;
                rot_direction = 0;
                break;
            case 'U':
                pane_num = z_pane;
                rot_direction = 1;
                break;
            }
        }

        // Looking at BACK of cube
        if ((h_angle > 225) && (h_angle < 315)) {
            switch (swipe_direction) {
            case 'D':
                pane_num = x_pane;
                rot_direction = 1;
                break;
            case 'U':
                pane_num = x_pane;
                rot_direction = 0;
                break;
            }
        }

        // Looking at RIGHT of cube
        if ((h_angle > 315) || (h_angle < 45)) {
            switch (swipe_direction) {
            case 'D':
                pane_num = z_pane;
                rot_direction = 1;
                break;
            case 'U':
                pane_num = z_pane;
                rot_direction = 0;
                break;
            }
        }
    }

    // Looking at BOTTOM of cube
    else if (v_angle < -45) {

        // Looking at BOTTOM FRONT of cube 
        if ((h_angle > 45) && (h_angle < 135)) {
            switch (swipe_direction) {
            case 'L':
                pane_num = z_pane;
                rot_direction = 1;
                break;
            case 'R':
                pane_num = z_pane;
                rot_direction = 0;
                break;
            case 'D':
                pane_num = x_pane;
                rot_direction = 0;
                break;
            case 'U':
                pane_num = x_pane;
                rot_direction = 1;
                break;
            }
        }

        // Looking at BOTTOM LEFT of cube  
        if ((h_angle > 135) && (h_angle < 225)) {
            switch (swipe_direction) {
            case 'L':
                pane_num = x_pane;
                rot_direction = 0;
                break;
            case 'R':
                pane_num = x_pane;
                rot_direction = 1;
                break;
            case 'D':
                pane_num = z_pane;
                rot_direction = 0;
                break;
            case 'U':
                pane_num = z_pane;
                rot_direction = 1;
                break;
            }
        }

        // Looking at BOTTOM BACK of cube
        if ((h_angle > 225) && (h_angle < 315)) {
            switch (swipe_direction) {
            case 'L':
                pane_num = z_pane;
                rot_direction = 0;
                break;
            case 'R':
                pane_num = z_pane;
                rot_direction = 1;
                break;
            case 'D':
                pane_num = x_pane;
                rot_direction = 1;
                break;
            case 'U':
                pane_num = x_pane;
                rot_direction = 0;
                break;
            }
        }

        // Looking at BOTTOM RIGHT of cube
        if ((h_angle > 315) || (h_angle < 45)) {
            switch (swipe_direction) {
            case 'L':
                pane_num = x_pane;
                rot_direction = 1;
                break;
            case 'R':
                pane_num = x_pane;
                rot_direction = 0;
                break;
            case 'D':
                pane_num = z_pane;
                rot_direction = 1;
                break;
            case 'U':
                pane_num = z_pane;
                rot_direction = 0;
                break;
            }
        }
    }

    if (pane_num != 10) {
        got_swipe = true;
    }
}

// Applying rotation to cube indexes for panes ( 0 = left/down, 1 = right/up)
void rotate_pane_indexes(int pane_num, int direction, vector<vector<vector<int>>>& panes) {
    vector<vector<int>> orig_pane;
    
    orig_pane = panes[pane_num];

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            // Turn in right/up direction
            if (direction == 1) {
                if (pane_num < 3) {
                    panes[pane_num][i][j] = orig_pane[j][2 - i];
                }
                else if ((pane_num > 2) && (pane_num < 6)) {
                    panes[pane_num][i][j] = orig_pane[j][2 - i];
                }
                else if (pane_num > 5) {
                    panes[pane_num][i][j] = orig_pane[j][2 - i];
                }
            }

            // Turn in left/down direction
            else if (direction == 0) {
                if (pane_num < 3) {
                    panes[pane_num][i][j] = orig_pane[2 - j][i];
                }
                else if ((pane_num > 2) && (pane_num < 6)) {
                    panes[pane_num][i][j] = orig_pane[2 - j][i];
                }
                else if (pane_num > 5) {
                    panes[pane_num][i][j] = orig_pane[2 - j][i];
                }
            }
        }
    }

    // If one of the z panes is tuning
    if (pane_num < 3) {
        // Update Y panes
        for (int i = 3; i < 6; i++) {
            for (int j = 0; j < 3; j++) {
                panes[i][pane_num][j] = panes[pane_num][i - 3][j];
            }
        }

        // Update X panes
        for (int i = 6; i < 9; i++) {
            for (int j = 0; j < 3; j++) {
                panes[i][pane_num][j] = panes[pane_num][j][i - 6];
            }
        }
    }

    // If one of the y panes is tuning
    if ((pane_num > 2) && (pane_num < 6)) {

        // Update Z panes
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                panes[i][pane_num % 3][j] = panes[pane_num][i][j];
            }
        }

        // Update X panes
        for (int i = 6; i < 9; i++) {
            for (int j = 0; j < 3; j++) {
                panes[i][j][pane_num % 3] = panes[pane_num][j][i % 3];
            }
        }
    }

    // If one of the x panes is tuning
    if (pane_num > 5) {
        
        // Update Z panes
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                panes[i][j][pane_num % 3] = panes[pane_num][i][j];
            }
        }

        // Update Y panes
        for (int i = 3; i < 6; i++) {
            for (int j = 0; j < 3; j++) {
                panes[i][j][pane_num%3] = panes[pane_num][j][i-3];
            }
        }
    }
}


// Saving current Rubik's Cube to save file
void save_cube(vector<vector<vector<int>>> panes, vector<glm::mat4> models, string file_name) {
    ofstream file(file_name);

    // Saving panes
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 3; j++) {
            for (int k = 0; k < 3; k++) {
                file << panes[i][j][k] << endl;
            }
        }
    }

    // Saving models
    for (int i = 0; i < 27; i++) {
        for (int j = 0; j < 4; j++) {
            for (int k = 0; k < 4; k++) {
                file << models[i][k][j] << endl;
            }
        }
    }

    // Saving cube colors
    for (int i = 0; i < 27; i++) {
        for (int j = 0; j < 6; j++) {
            file << cube_colors[i][j] << endl;
        }
    }

    file.close();
}

// Loading Runik's Cube from save file
void load_cube(vector<vector<vector<int>>> &panes, vector<glm::mat4> & models, string file_name) {
    fstream file;
    file.open(file_name, ios::in);
    string line;

    if (file.is_open()) { //checking whether the file is open
        // Loading panes
        for (int i = 0; i < 9; i++) {
            for (int j = 0; j < 3; j++) {
                for (int k = 0; k < 3; k++) {
                    getline(file, line);
                    panes[i][j][k] = stoi(line);
                }
            }
        }

        // Loading models
        for (int i = 0; i < 27; i++) {
            for (int j = 0; j < 4; j++) {
                for (int k = 0; k < 4; k++) {
                    getline(file, line);
                    models[i][k][j] = stof(line);
                }
            }
        }

        // Loading cube colors
        for (int i = 0; i < 27; i++) {
            for (int j = 0; j < 6; j++) {
                getline(file, line);
                cube_colors[i][j] = stoi(line);
            }
        }
        update_rubiks_faces(models);
        file.close();
    }
    else {
        cout << "File '" << file_name << "' was not found. Initizializing to default Rubik's Cube layout." << endl;
    }
}


int main() {
    char load_ans;
    string load_file_name = "save.txt";
    bool load = false;

    cout << "Welcome to the Interactive Rubik's Cube!" << endl;
    cout << "To rotate the entire Rubik's Cube for different viewing angles, click anywhere in the window not on the cub eand drag." << endl;
    cout << "To rotate a pane, click and hold on one of the pane's 4 sides, not the face itself, and drag towards the direction you want to rotate." << endl;
    cout << "To save the current Rubik's Cube layout to a save file, which you can load back in later on, press the 'S' key at any time." << endl;
    cout << "Would you like to load cube from save file? (y/n): ";
    cin >> load_ans;

    if (load_ans == 'y') {
        cout << "Input save file name to load: ";
        cin >> load_file_name;
        load = true;
    }
    else {
        cout << "Input how many turns you would like the Rubik's Cube to be shuffled: ";
        cin >> init_count;
        init = true;
    }
    

    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);

    if (GLEW_OK != glewInit()) {
        // GLEW failed!
        exit(1);
    }

    glEnable(GL_DEPTH_TEST);

    Shader ourShader("shader.vs", "shader.fs"); // you can name your shader files however you like

    vector<float> c;
    vector<float> cubes;
    vector<int> colors;;
    vector<glm::mat4> models;
    vector<vector<vector<int>>> panes;
  

    int cube_num = 0;
    int red_count = 0;
    int blue_count = 0;
    int orange_count = 0;
    int yellow_count = 0;
    int green_count = 0;
    int white_count = 0;

    vector<float> back = { 0.0f, 0.0f, 0.0f };
    vector<float> front = { 0.0f, 0.0f, 0.0f };
    vector<float> left = { 0.0f, 0.0f, 0.0f };
    vector<float> right = { 0.0f, 0.0f, 0.0f };
    vector<float> bottom = { 0.0f, 0.0f, 0.0f };
    vector<float> top = { 0.0f, 0.0f, 0.0f };

    vector<float> back_face;
    vector<float> front_face;
    vector<float> left_face;
    vector<float> right_face;
    vector<float> bottom_face;
    vector<float> top_face;


    // Initial setting for what cubes are in what panes
    vector<vector<int> > back_z = { {0,1,2}, {3,4,5}, {6,7,8} }; // back pane, bottom to top
    panes.push_back(back_z);
    vector<vector<int> > middle_z = { {9,10,11}, {12,13,14}, {15,16,17} }; // middle front facing pane, bottom to top
    panes.push_back(middle_z);
    vector<vector<int> > front_z = { {18,19,20}, {21,22,23}, {24,25,26} }; // front pane, bottom to top
    panes.push_back(front_z);

    vector<vector<int> > bottom_y = { {0,1,2}, {9,10,11}, {18,19,20} }; // bottom pane, back to front
    panes.push_back(bottom_y);
    vector<vector<int> > middle_y = { {3,4,5}, {12,13,14}, {21,22,23} }; // middle floor facing pane, back to front
    panes.push_back(middle_y);
    vector<vector<int> > top_y = { {6,7,8}, {15,16,17}, {24,25,26} }; // top pane, back to front
    panes.push_back(top_y);

    vector<vector<int> > left_x = { {0,3,6}, {9,12,15}, {18,21,24} }; // left pane, back to front
    panes.push_back(left_x);
    vector<vector<int> > middle_x = { {1,4,7}, {10,13,16}, {19,22,25} }; // middle side facing pane, back to front
    panes.push_back(middle_x);
    vector<vector<int> > right_x = { {2,5,8}, {11,14,17}, {20,23,26} }; // right pane, back to front
    panes.push_back(right_x);

    // set up vertex data (and buffer(s)) and configure vertex attributes
    for (int z = -1; z < 2; z++) {
        for (int y = -1; y < 2; y++) {
            for (int x = -1; x < 2; x++) {
                
                // Change each of the 6th cube colors slightly so that each cube has unique colors so you can decifer which cube was clicked on
                switch (z) {
                case -1:
                    back = {1-(orange_count * 0.004f), 0.65f, 0};
                    front = {0, 0, 0};
                    sorted_cube_colors[0].insert(sorted_cube_colors[0].begin(), (int)(round(back[0] * 255) + round(back[1] * 255) * 256 + round(back[2] * 255) * 256 * 256));
                    orange_count++;
                    break;
                case 0:
                    back = {0, 0, 0};
                    front = {0, 0, 0};
                    break;
                case 1:
                    back = {0, 0, 0};
                    front = {1-(red_count * 0.004f), 0, 0};
                    sorted_cube_colors[1].insert(sorted_cube_colors[1].begin(), (int)(round(front[0] * 255) + round(front[1] * 255) * 256 + round(front[2] * 255) * 256 * 256));
                    red_count++;
                    break;
                }

                switch (y) {
                case -1:
                    top = {0, 0, 0};
                    bottom = {1-(yellow_count * 0.004f), 1, 0};
                    sorted_cube_colors[4].insert(sorted_cube_colors[4].begin(), (int)(round(bottom[0] * 255) + round(bottom[1] * 255) * 256 + round(bottom[2] * 255) * 256 * 256));
                    yellow_count++;
                    break;
                case 0:
                    top = {0, 0, 0};
                    bottom = {0, 0, 0};
                    break;
                case 1:
                    top = {1-(white_count * 0.004f), 1, 1};
                    bottom = {0, 0, 0};
                    sorted_cube_colors[5].insert(sorted_cube_colors[5].begin(), (int)(round(top[0] * 255) + round(top[1] * 255) * 256 + round(top[2] * 255) * 256 * 256));
                    white_count++;
                    break;
                }

                switch (x) {
                case -1:
                    left = {0, 1-(green_count * 0.004f), 0 };
                    right = {0, 0, 0 };
                    sorted_cube_colors[2].insert(sorted_cube_colors[2].begin(), (int)(round(left[0] * 255) + round(left[1] * 255) * 256 + round(left[2] * 255) * 256 * 256));
                    green_count++;
                    break;
                case 0:
                    left = {0, 0, 0};
                    right = {0, 0, 0};
                    break;
                case 1:
                    left = {0, 0, 0};
                    right = {0, 0, 1 - (blue_count * 0.004f)};
                    sorted_cube_colors[3].insert(sorted_cube_colors[3].begin(), (int)(round(right[0] * 255) + round(right[1] * 255) * 256 + round(right[2] * 255) * 256 * 256));
                    blue_count++;
                    break;
                }

                // Vertex array for one cube
                c = {
                    // Back 
                    -0.5f, -0.5f, -0.5f, back[0], back[1], back[2],
                     0.5f, -0.5f, -0.5f, back[0], back[1], back[2],
                     0.5f,  0.5f, -0.5f, back[0], back[1], back[2],
                     0.5f,  0.5f, -0.5f, back[0], back[1], back[2],
                    -0.5f,  0.5f, -0.5f, back[0], back[1], back[2],
                    -0.5f, -0.5f, -0.5f, back[0], back[1], back[2],

                    // Front
                    -0.5f, -0.5f,  0.5f,  front[0], front[1], front[2],
                    0.5f, -0.5f,  0.5f, front[0], front[1], front[2],
                    0.5f,  0.5f,  0.5f, front[0], front[1], front[2],
                    0.5f,  0.5f,  0.5f, front[0], front[1], front[2],
                    -0.5f,  0.5f,  0.5f, front[0], front[1], front[2],
                    -0.5f, -0.5f,  0.5f, front[0], front[1], front[2],

                    // Left
                    -0.5f,  0.5f,  0.5f, left[0], left[1], left[2],
                    -0.5f,  0.5f, -0.5f, left[0], left[1], left[2],
                    -0.5f, -0.5f, -0.5f, left[0], left[1], left[2],
                    -0.5f, -0.5f, -0.5f, left[0], left[1], left[2],
                    -0.5f, -0.5f,  0.5f, left[0], left[1], left[2],
                    -0.5f,  0.5f,  0.5f, left[0], left[1], left[2],

                    // Right
                    0.5f,  0.5f,  0.5f, right[0], right[1], right[2],
                    0.5f,  0.5f, -0.5f, right[0], right[1], right[2],
                    0.5f, -0.5f, -0.5f, right[0], right[1], right[2],
                    0.5f, -0.5f, -0.5f, right[0], right[1], right[2],
                    0.5f, -0.5f,  0.5f, right[0], right[1], right[2],
                    0.5f,  0.5f,  0.5f, right[0], right[1], right[2],

                    // Bottom
                    -0.5f, -0.5f, -0.5f, bottom[0], bottom[1], bottom[2],
                     0.5f, -0.5f, -0.5f, bottom[0], bottom[1], bottom[2],
                     0.5f, -0.5f,  0.5f, bottom[0], bottom[1], bottom[2],
                     0.5f, -0.5f,  0.5f, bottom[0], bottom[1], bottom[2],
                    -0.5f, -0.5f,  0.5f, bottom[0], bottom[1], bottom[2],
                    -0.5f, -0.5f, -0.5f, bottom[0], bottom[1], bottom[2],

                    // Top
                    -0.5f,  0.5f, -0.5f, top[0], top[1], top[2],
                     0.5f,  0.5f, -0.5f, top[0], top[1], top[2],
                     0.5f,  0.5f,  0.5f, top[0], top[1], top[2],
                     0.5f,  0.5f,  0.5f, top[0], top[1], top[2],
                    -0.5f,  0.5f,  0.5f, top[0], top[1], top[2],
                    -0.5f,  0.5f, -0.5f, top[0], top[1], top[2]
                };
                
                // Adding vertex array of one cube to end of big array that has all the cubes
                cubes.insert(cubes.end(), c.begin(), c.end());

                // Adding to array of cube models
                models.push_back(glm::translate(glm::mat4(1.0f), glm::vec3(x * 1.05, y * 1.05, z * 1.05)));

                // Setting each cube's 6 colors
                colors = {
                    (int)(round(back[0] * 255) + round(back[1] * 255) * 256 + round(back[2] * 255) * 256 * 256),
                    (int)(round(front[0] * 255) + round(front[1] * 255) * 256 + round(front[2] * 255) * 256 * 256),
                    (int)(round(left[0] * 255) + round(left[1] * 255) * 256 + round(left[2] * 255) * 256 * 256),
                    (int)(round(right[0] * 255) + round(right[1] * 255) * 256 + round(right[2] * 255) * 256 * 256),
                    (int)(round(bottom[0] * 255) + round(bottom[1] * 255) * 256 + round(bottom[2] * 255) * 256 * 256),
                    (int)(round(top[0] * 255) + round(top[1] * 255) * 256 + round(top[2] * 255) * 256 * 256)           
                };
     
                cube_colors.push_back(colors);
                cube_num++;

            }
        }
    }

    // If user specified to load a save file, replace variables with saved variables
    if (load) {
        load_cube(panes, models, load_file_name);
    }

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, cubes.size() * sizeof(float), &cubes[0], GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    
    bool rotate = false;
    float currentAngle = 0.0f;
    
    srand(std::time(nullptr));
    string pane_name;
    glm::vec3 rot_axis;
    vector<int> flattened_pane;

    // render loop
    while (!glfwWindowShouldClose(window)) {
        glfwGetWindowSize(window, &SCR_WIDTH, &SCR_HEIGHT);

        // input
        processInput(window);

        // render
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer now!


        // get matrix's uniform location and set matrix
        ourShader.use();
        
        // pass projection matrix to shader (note that in this case it could change every frame)
        glm::mat4 projection = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        ourShader.setMat4("projection", projection);

        // camera/view transformation
        glm::mat4 view = glm::lookAt(cameraPos, glm::vec3(0.0f, 0.0f, 0.0f), cameraUp);
        ourShader.setMat4("view", view);

        // render boxes
        glBindVertexArray(VAO);


        // If user presses 'S' key, save all variables to save file
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            string save_file_name = "save.txt";
            cout << "You have pressed 's' to save. Input save file name: ";
            cin >> save_file_name;
            save_cube(panes, models, save_file_name);
            return 0;
        }

        // When mouse click is released, determine if it was on the Rubik's Cube and try to rotate a pane
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
            if (firstMouse == false){
                if ((color_ID != 0) && (color_ID != 5066035) && (got_swipe == false) && (init == false)) {
                    get_rotation_from_swipe(initial_x, initial_y, lastX, SCR_HEIGHT - lastY, yaw, pitch, panes);
                }
            }
            firstMouse = true;
        }

        // When a pane is not being rotated
        if (rotate == false) {

            // Shuffle cube randomly for certain amount of times if init is true
            if (init == true) {

                if (init_count == 0) {
                    init = false;
                }
                else {
                    pane_num = rand() % 9;
                    flattened_pane = flatten(panes[pane_num]);
                    rot_direction = rand() % 2;
                    init_count--;
                    rotate = true;
                }
            }

            // If not initializing, get rotation from user swipe if they swiped
            else if (got_swipe == true) {
                flattened_pane = flatten(panes[pane_num]);
                rotate = true;
            }

            if (pane_num < 3) {
                rot_axis = rot_Z_right;
                if (rot_direction == 0) {
                    rot_axis = rot_Z_left;
                }
            }
            else if ((pane_num > 2) && (pane_num < 6)) {
                rot_axis = rot_Y_right;
                if (rot_direction == 0) {
                    rot_axis = rot_Y_left;
                }
            }
            else if (pane_num > 5) {
                rot_axis = rot_X_up;
                if (rot_direction == 0) {
                    rot_axis = rot_X_down;
                }
            }
        }

        // If rotation is complete
        if (currentAngle >= 90.0f) {
            rotate = false;
            currentAngle = 0.0f;
            update_cube_colors(flattened_pane,rot_axis);
            rotate_pane_indexes(pane_num, rot_direction, panes);
            update_rubiks_faces(models);
            got_swipe = false;
            if (check_complete()) {
                cout << "Congratulations! You have solved the Rubik's Cube!" << endl;
            }
        }

        // If rotation is true and rotation is not complete, keep rotating
        else if (rotate) {
            currentAngle += rotation_rate;
        }
       
        for (unsigned int i = 0; i < 27; i++) {
            // calculate the model matrix for each object and pass it to shader before drawing
            glm::mat4 model = models[i];

            // If pane is supposed to be rotating, find the cubes within that pane and rotate them
            if (rotate) {
                if (find(flattened_pane.begin(), flattened_pane.end(), i) != flattened_pane.end()) {
                    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(rotation_rate), rot_axis);
                    model = rotation * model;
                    models[i] = model;
                }
            }

            ourShader.setMat4("model", models[i]);
            
            // Draw cubes
            glDrawArrays(GL_TRIANGLES, (i * 36), 36);
        }

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
    return 0;
}


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    // If mouse was clicked
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {

        // If mouse wasn't being clicked in the frame before
        if (firstMouse) {
            initial_x = xpos;
            initial_y = SCR_HEIGHT - ypos;
            lastX = xpos;
            lastY = ypos;

            // Read pixel color of where mouse was clicked
            glFlush();
            glFinish();

            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            unsigned char data[4];
            glReadPixels(initial_x, initial_y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);

            color_ID = (int)data[0] + (int)data[1] * 256 + (int)data[2] * 256 * 256;
            
            // Get which cube and what face of the cube was clicked on based on color ID
            if ((color_ID != 0) && (color_ID != 5066035)) {
                current_cube_and_face = get_cube_ID(color_ID);
            }
            firstMouse = false;
        }
        
        // If user clicked on background, rotate camera
        if (color_ID == 5066035) {
            float xoffset = xpos - lastX;
            float yoffset = ypos - lastY;
            lastX = xpos;
            lastY = ypos;

            float sensitivity = 0.3f;
            xoffset *= sensitivity;
            yoffset *= sensitivity;

            yaw += xoffset;
            pitch += yoffset;

            // make sure that when pitch is out of bounds, screen doesn't get flipped
            if (pitch > 89.0f)
                pitch = 89.0f;
            if (pitch < -89.0f)
                pitch = -89.0f;

            // Rotate camera about origin, like it is orbiting around the center
            const float radius = 10.0f;
            float camX = cos(glm::radians(yaw)) * cos(glm::radians(pitch)) * radius;
            float camY = sin(glm::radians(pitch)) * radius;
            float camZ = sin(glm::radians(yaw)) * cos(glm::radians(pitch)) * radius;

            cameraPos = glm::vec3(camX, camY, camZ);
        }

        else {
            lastX = xpos;
            lastY = ypos;
        }
    }
}