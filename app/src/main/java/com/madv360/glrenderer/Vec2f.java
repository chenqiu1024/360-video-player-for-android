package com.madv360.glrenderer;

import java.io.Serializable;

/**
 * Created by qiudong on 16/7/18.
 */
public class Vec2f implements Serializable {
    public Vec2f(float x, float y) {
        this.x = x;
        this.y = y;
    }

    public float x;
    public float y;
}
