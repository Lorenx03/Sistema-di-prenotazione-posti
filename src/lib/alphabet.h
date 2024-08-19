enum alphabetChars{
    A = 0,
    B,
    C,
    D,
    E,
    F,
    G,
    H,
    I,
    J,
    K,
    L,
    M,
    N,
    O,
    P,
    Q,
    R,
    S,
    T,
    U,
    V,
    W,
    X,
    Y,
    Z,
};


struct letter{
    char font[5][5];
    int width;
};

const struct letter alphabet[26] = {
    {
        .font = {
            {' ', '#', '#', ' ', ' '},
            {'#', ' ', ' ', '#', ' '},
            {'#', '#', '#', '#', ' '},
            {'#', ' ', ' ', '#', ' '},
            {'#', ' ', ' ', '#', ' '}
        },
        .width = 4
    },
    {
        .font = {
            {'#', '#', '#', ' ', ' '},
            {'#', ' ', ' ', '#', ' '},
            {'#', '#', '#', ' ', ' '},
            {'#', ' ', ' ', '#', ' '},
            {'#', '#', '#', ' ', ' '}
        },
        .width = 4
    },
    {
        .font = {
            {' ', '#', '#', '#', ' '},
            {'#', ' ', ' ', ' ', ' '},
            {'#', ' ', ' ', ' ', ' '},
            {'#', ' ', ' ', ' ', ' '},
            {' ', '#', '#', '#', ' '}
        },
        .width = 4
    },
    {
        .font = {
            {'#', '#', '#', ' ', ' '},
            {'#', ' ', ' ', '#', ' '},
            {'#', ' ', ' ', '#', ' '},
            {'#', ' ', ' ', '#', ' '},
            {'#', '#', '#', ' ', ' '}
        },
        .width = 4
    },
    {
        .font = {
            {'#', '#', '#', ' ', ' '},
            {'#', ' ', ' ', ' ', ' '},
            {'#', '#', '#', ' ', ' '},
            {'#', ' ', ' ', ' ', ' '},
            {'#', '#', '#', ' ', ' '}
        },
        .width = 3
    },
    {
        .font = {
            {'#', '#', '#', ' ', ' '},
            {'#', ' ', ' ', ' ', ' '},
            {'#', '#', '#', ' ', ' '},
            {'#', ' ', ' ', ' ', ' '},
            {'#', ' ', ' ', ' ', ' '}
        },
        .width = 3
    },
    {
        .font = {
            {' ', '#', '#', '#', ' '},
            {'#', ' ', ' ', ' ', ' '},
            {'#', ' ', '#', '#', ' '},
            {'#', ' ', ' ', '#', ' '},
            {' ', '#', '#', '#', ' '}
        },
        .width = 4
    },
    {
        .font = {
            {'#', ' ', ' ', '#', ' '},
            {'#', ' ', ' ', '#', ' '},
            {'#', '#', '#', '#', ' '},
            {'#', ' ', ' ', '#', ' '},
            {'#', ' ', ' ', '#', ' '}
        },
        .width = 4
    },
    {
        .font = {
            {'#', ' ', ' ', ' ', ' '},
            {'#', ' ', ' ', ' ', ' '},
            {'#', ' ', ' ', ' ', ' '},
            {'#', ' ', ' ', ' ', ' '},
            {'#', ' ', ' ', ' ', ' '}
        },
        .width = 1
    },
    {
        .font = {
            {' ', ' ', ' ', '#', ' '},
            {' ', ' ', ' ', '#', ' '},
            {' ', ' ', ' ', '#', ' '},
            {'#', ' ', ' ', '#', ' '},
            {' ', '#', '#', ' ', ' '}
        },
        .width = 4
    },
    {
        .font = {
            {'#', ' ', ' ', '#', ' '},
            {'#', ' ', '#', '#', ' '},
            {'#', '#', ' ', ' ', ' '},
            {'#', ' ', '#', ' ', ' '},
            {'#', ' ', ' ', '#', ' '}
        },
        .width = 4
    },
    {
        .font = {
            {'#', ' ', ' ', ' ', ' '},
            {'#', ' ', ' ', ' ', ' '},
            {'#', ' ', ' ', ' ', ' '},
            {'#', ' ', ' ', ' ', ' '},
            {'#', '#', '#', '#', ' '}
        },
        .width = 3
    },
    {
        .font = {
            {'#', ' ', ' ', ' ', '#'},
            {'#', '#', ' ', '#', '#'},
            {'#', ' ', '#', ' ', '#'},
            {'#', ' ', ' ', ' ', '#'},
            {'#', ' ', ' ', ' ', '#'}
        },
        .width = 5
    },
    {
        .font = {
            {'#', ' ', ' ', ' ', '#'},
            {'#', '#', ' ', ' ', '#'},
            {'#', ' ', '#', ' ', '#'},
            {'#', ' ', ' ', '#', '#'},
            {'#', ' ', ' ', ' ', '#'}
        },
        .width = 5
    },
    {
        .font = {
            {' ', '#', '#', ' ', ' '},
            {'#', ' ', ' ', '#', ' '},
            {'#', ' ', ' ', '#', ' '},
            {'#', ' ', ' ', '#', ' '},
            {' ', '#', '#', ' ', ' '}
        },
        .width = 4
    },
    {
        .font = {
            {'#', '#', '#', ' ', ' '},
            {'#', ' ', ' ', '#', ' '},
            {'#', '#', '#', ' ', ' '},
            {'#', ' ', ' ', ' ', ' '},
            {'#', ' ', ' ', ' ', ' '}
        },
        .width = 4
    },
    {
        .font = {
            {' ', '#', '#', ' ', ' '},
            {'#', ' ', ' ', '#', ' '},
            {'#', ' ', ' ', '#', ' '},
            {'#', ' ', ' ', '#', ' '},
            {' ', '#', '#', '#', ' '}
        },
        .width = 4
    },
    {
        .font = {
            {'#', '#', '#', ' ', ' '},
            {'#', ' ', ' ', '#', ' '},
            {'#', '#', '#', ' ', ' '},
            {'#', ' ', '#', ' ', ' '},
            {'#', ' ', ' ', '#', ' '}
        },
        .width = 4
    },
    {
        .font = {
            {' ', '#', '#', '#', ' '},
            {'#', ' ', ' ', ' ', ' '},
            {' ', '#', '#', ' ', ' '},
            {' ', ' ', ' ', '#', ' '},
            {'#', '#', '#', ' ', ' '}
        },
        .width = 4
    },
    {
        .font = {
            {'#', '#', '#', ' ', ' '},
            {' ', '#', ' ', ' ', ' '},
            {' ', '#', ' ', ' ', ' '},
            {' ', '#', ' ', ' ', ' '},
            {' ', '#', ' ', ' ', ' '}
        },
        .width = 3
    },
    {
        .font = {
            {'#', ' ', ' ', '#', ' '},
            {'#', ' ', ' ', '#', ' '},
            {'#', ' ', ' ', '#', ' '},
            {'#', ' ', ' ', '#', ' '},
            {' ', '#', '#', ' ', ' '}
        },
        .width = 4
    },
    {
        .font = {
            {'#', ' ', ' ', ' ', '#'},
            {'#', ' ', ' ', ' ', '#'},
            {' ', '#', ' ', '#', ' '},
            {' ', '#', ' ', '#', ' '},
            {' ', ' ', '#', ' ', ' '}
        },
        .width = 5
    },
    {
        .font = {
            {'#', ' ', ' ', ' ', '#'},
            {'#', ' ', '#', ' ', '#'},
            {'#', ' ', '#', ' ', '#'},
            {'#', ' ', '#', ' ', '#'},
            {' ', '#', ' ', '#', ' '}
        },
        .width = 5
    },
    {
        .font = {
            {'#', ' ', ' ', ' ', '#'},
            {' ', '#', ' ', '#', '#'},
            {' ', ' ', '#', ' ', ' '},
            {' ', '#', ' ', '#', ' '},
            {'#', ' ', ' ', ' ', '#'}
        },
        .width = 5
    },
    {
        .font = {
            {'#', ' ', ' ', ' ', '#'},
            {' ', '#', ' ', '#', ' '},
            {' ', ' ', '#', ' ', ' '},
            {' ', ' ', '#', ' ', ' '},
            {' ', ' ', '#', ' ', ' '}
        },
        .width = 5
    },
    {
        .font = {
            {'#', '#', '#', '#', ' '},
            {' ', ' ', ' ', '#', ' '},
            {' ', '#', '#', ' ', ' '},
            {'#', ' ', ' ', ' ', ' '},
            {'#', '#', '#', '#', ' '}
        },
        .width = 4
    },
};