DISTANCE_NUM_ANGLES = 11;
g_pos_ix = DISTANCE_NUM_ANGLES//2;
g_pos_ix_previous = g_pos_ix;

def sweep(pos_ix, pos_ix_previous):
    if (pos_ix < pos_ix_previous):
        pos_ix_previous = pos_ix;
        pos_ix-=1;
        if (pos_ix < 0):
            pos_ix = 1;

    else:
        pos_ix_previous = pos_ix;
        pos_ix+=1;
        if (pos_ix == DISTANCE_NUM_ANGLES):
            pos_ix = DISTANCE_NUM_ANGLES - 2;

    return (pos_ix, pos_ix_previous)

for i in range(50):
    (g_pos_ix, g_pos_ix_previous) = sweep(g_pos_ix, g_pos_ix_previous);
    print(g_pos_ix);
