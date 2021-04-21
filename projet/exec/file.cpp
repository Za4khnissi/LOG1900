
int main(int argc, char const *argv[])
{
    int prevButton;

    while (true)
    {
        bouton = boutonPressed();
        if(bouton == REPEAT)
        {
            prevButton = bouton;
        }
        
        switch (bouton)
        {

        case UN:
            /* code */
            _delay_ms(1000);
            break;
        case DEUX:
            /* code */
            _delay_ms(500);

            break;
        case TROIS:
            /* code */
            _delay_ms(250);
            break;
        case /* constant-expression */:
            /* code */
            break;
        
        default:
            _delay_ms(1000);
            break;
        /* code */
        }

        modeDetection();

        switch (displayMode)
        {
        case R:
            display();
            break;

        case V:
            if(au moins une distance a change)
                display();

            break;

        case C:
            if(au moins une categorie a change)
                display();

        default:
            display();
            break;
        }
    }

    return 0;
}




