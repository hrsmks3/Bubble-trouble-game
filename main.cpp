#include <simplecpp>
#include "shooter.h"
#include "bubble.h"

/* Simulation Vars */
const double STEP_TIME = 0.02;

/* Game Vars */
const int PLAY_Y_HEIGHT = 450;
const int LEFT_MARGIN = 70;
const int TOP_MARGIN = 20;
const int BOTTOM_MARGIN = (PLAY_Y_HEIGHT+TOP_MARGIN);


void move_bullets(vector<Bullet> &bullets){
    // move all bullets
    for(unsigned int i=0; i<bullets.size(); i++){
        if(!bullets[i].nextStep(STEP_TIME)){
            bullets.erase(bullets.begin()+i);
        }
    }
}

void move_bubbles(vector<Bubble> &bubbles){
    // move all bubbles
    for (unsigned int i=0; i < bubbles.size(); i++)
    {
        bubbles[i].nextStep(STEP_TIME);
    }
}


bool bubble_collision(vector<Bullet> &bullets, vector<Bubble> &bubbles)
{
    // bubble and bullet collision effect

    for(unsigned int i = 0; i < bullets.size(); i++)
    {
        for(unsigned int j = 0; j < bubbles.size(); j++)
        {
            if(((abs(bullets[i].get_center_x() - bubbles[j].get_center_x())) < (bullets[i].get_width()/2.0 + bubbles[j].get_radius()))
              &&
              ((abs(bullets[i].get_center_y() - bubbles[j].get_center_y())) < (bullets[i].get_height()/2.0 + bubbles[j].get_radius())))
            {
                 if(bubbles[j].get_radius() == 10)
                 {
                     bullets.erase(bullets.begin()+i); //bullet is removed after collision of bullet with small bubble
                     bubbles.erase(bubbles.begin()+j); //bubble is removed after collision of bullet with small bubble
                     return true;
                 }
                 else
                 {
                     //Splitting of big bubble into two smaller bubbles after bullet hit
                     bubbles.push_back(Bubble(bubbles[j].get_center_x(), bubbles[j].get_center_y(), BUBBLE_RADIUS_THRESHOLD, -bubbles[j].get_vx(),
                     bubbles[j].get_vy(), COLOR(255,105,180)));
                     bubbles.push_back(Bubble(bubbles[j].get_center_x(), bubbles[j].get_center_y(), BUBBLE_RADIUS_THRESHOLD, bubbles[j].get_vx(),
                     bubbles[j].get_vy(), COLOR(255,105,180)));
                     //Big bubble is removed
                     bubbles.erase(bubbles.begin()+j);
                     //Bullet is removed
                     bullets.erase(bullets.begin()+i);
                     return true;
                 }
            }
        }
    }


    return false;
}
bool shooter_collision(vector<Bubble> &bubbles, Shooter &shooter)
{
    //Shooter and bubble collision effect
    for(unsigned int i = 0; i < bubbles.size(); i++)
    {
        if(((abs(pow(pow(bubbles[i].get_center_x()-shooter.get_head_center_x(),2.0)+pow(bubbles[i].get_center_y()-shooter.get_head_center_y(),2.0),0.5)))
        <
        (bubbles[i].get_radius()+shooter.get_head_radius()))
        ||
        (((abs(bubbles[i].get_center_x() - shooter.get_body_center_x())) < (bubbles[i].get_radius() + shooter.get_body_width()/2.0))
        &&
        ((abs(bubbles[i].get_center_y() - shooter.get_body_center_y())) < (bubbles[i].get_radius() + shooter.get_body_height()/2.0))))
        {
           //Bubble rebounds after collision with shooter
           //Bubble with opposite velocity is created after collision with shooter
           bubbles.push_back(Bubble(bubbles[i].get_center_x(), bubbles[i].get_center_y(), bubbles[i].get_radius(), -bubbles[i].get_vx(),
           -bubbles[i].get_vy(), COLOR(255,105,180)));
           //Initial bubble is removed after collision with shooter
           bubbles.erase(bubbles.begin()+i);
           return true;
        }
    }
    return false;
}


vector<Bubble> create_bubbles()
{
    // create initial bubbles in the game
    vector<Bubble> bubbles;
    bubbles.push_back(Bubble(WINDOW_X/2.0, BUBBLE_START_Y, BUBBLE_DEFAULT_RADIUS, -BUBBLE_DEFAULT_VX, 0, COLOR(255,105,180)));
    bubbles.push_back(Bubble(WINDOW_X/4.0, BUBBLE_START_Y, BUBBLE_DEFAULT_RADIUS, BUBBLE_DEFAULT_VX, 0, COLOR(255,105,180)));
    bubbles.push_back(Bubble(WINDOW_X/10.0, BUBBLE_START_Y, BUBBLE_DEFAULT_RADIUS, BUBBLE_DEFAULT_VX, 10, COLOR(255,105,180)));
    return bubbles;
}



int main()
{
    initCanvas("Bubble Trouble", WINDOW_X, WINDOW_Y);

    Line b1(0, PLAY_Y_HEIGHT, WINDOW_X, PLAY_Y_HEIGHT);
    b1.setColor(COLOR(0, 0, 255));

    string msg_cmd("Cmd: _");
    Text charPressed(LEFT_MARGIN, BOTTOM_MARGIN, msg_cmd);

    int score = 48; //Initialized ASCII value of score number
    string msg_score("Score: ");
    Text Score(WINDOW_X - LEFT_MARGIN, BOTTOM_MARGIN, msg_score);

    int set_time_1 = 48;//Initialized ASCII value of first digit of time
    int set_time_2 = 48;//Initialized ASCII value of second digit of time
    int iterations = 0; //No. of iterations of main game loop
    string msg_time("Time:  /50");
    Text Time(LEFT_MARGIN, TOP_MARGIN, msg_time);

    int health = 51;//Initialized ASCII value of health points
    string msg_health("Health: /3");
    Text Health(WINDOW_X - LEFT_MARGIN, TOP_MARGIN, msg_health);

    // Intialize the shooter
    Shooter shooter(SHOOTER_START_X, SHOOTER_START_Y, SHOOTER_VX);

    // Initialize the bubbles
    vector<Bubble> bubbles = create_bubbles();

    // Initialize the bullets (empty)
    vector<Bullet> bullets;

    XEvent event;



    // Main game loop
    while (true)
    {

        iterations++;//
        //Game is over after 50 seconds

        if(iterations == 1500)
        {
            set_time_2 = 48;
            set_time_1++;
            msg_time[msg_time.length() - 5] = set_time_1;
            msg_time[msg_time.length() - 4] = set_time_2;
            Time.setMessage(msg_time);//displays final time

            //Print "Game Over" after game is lost
            Text LOSE_MSG(250, 250, "Game Over");
            LOSE_MSG.setColor(COLOR(229, 43, 43));
            LOSE_MSG.setFill(true);
            LOSE_MSG.imprint();
            //Game window closes after 5 seconds after game over
            wait(5);
            break;
        }
        //Time incrementation and display
        if(iterations % 30 == 0)
        {
            //Incrementation of second digit of time
            set_time_2++;
            msg_time[msg_time.length() - 4] = set_time_2;
            Time.setMessage(msg_time);

        }
        if(iterations % 300 == 0)
        {
            //Incrementation of first digit of time after second digit takes 9 value andsecond bit is reset to zero after first digit is incremented
            set_time_2 = 48;
            set_time_1++;
            msg_time[msg_time.length() - 5] = set_time_1;
            msg_time[msg_time.length() - 4] = set_time_2;
            Time.setMessage(msg_time);
        }

        bool pendingEvent = checkEvent(event);
        if (pendingEvent)
        {

            // Get the key pressed
            char c = charFromEvent(event);
            msg_cmd[msg_cmd.length() - 1] = c;
            charPressed.setMessage(msg_cmd);

            // Update the shooter
            if(c == 'a')
                shooter.move(STEP_TIME, true);
            else if(c == 'd')
                shooter.move(STEP_TIME, false);
            else if(c == 'w')
                bullets.push_back(shooter.shoot());
            else if(c == 'q')
                return 0;
        }
        //Sets and displays initial health point as 3
        msg_health[msg_health.length() - 3] = health;
        Health.setMessage(msg_health);

        //Sets and displays initial score as zero
        msg_score[msg_score.length() - 1] = score;
        Score.setMessage(msg_score);

        //Sets initial time as zero and displays it
        msg_time[msg_time.length() - 5] = set_time_1;
        msg_time[msg_time.length() - 4] = set_time_2;
        Time.setMessage(msg_time);

        // Update the bubbles
        move_bubbles(bubbles);

        // Update the bullets
        move_bullets(bullets);

        // Game over after collision between shooter and bubble

        if(shooter_collision(bubbles, shooter))
        {
            //Reduces Health point by 1 after shooter and bubble collision and displays it
            health--;
            msg_health[msg_health.length() - 3] = health;
            Health.setMessage(msg_health);

            if(health == 48)
            {
                //Print "Game Over" after Shooter is dead
                //Game is lost after shooter is dead
                Text LOSE_MSG(250, 250, "Game Over");
                LOSE_MSG.setColor(COLOR(229, 43, 43));
                LOSE_MSG.setFill(true);
                LOSE_MSG.imprint();
                //Game window closes after 5 seconds after shooter is dead
                wait(5);
                break;
            }
        }


        if(bubble_collision(bullets, bubbles))
        {
            score++;//Score is incremented after collision between bubble and bullet

            //Displays incremented score
            msg_score[msg_score.length() - 1] = score;
            Score.setMessage(msg_score);

            if(bubbles.size() == 0)
            {
                //Print "Congratulations!!" after winning game
                Text WIN_MSG(250, 250, "Congratulations!!");
                WIN_MSG.setColor(COLOR(128,255,0));
                WIN_MSG.setFill(true);
                WIN_MSG.imprint();
                //Game window closes after 5 seconds after game is win
                wait(5);
                break;
            }
        }


        wait(STEP_TIME);
    }

}


