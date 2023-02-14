#include <glm/gtx/euler_angles.hpp>
#include "AnmVM.h"
#include "AnmManager.h"
#include <NSEngine.h>
#include <Engine.hpp>

void AnmVM::draw(NSEngine::SpriteBatch* sb)
{
    /* CHECK IF THE VM IS VISIBLE */
    if (bitflags.activeFlags != ANMVM_ACTIVE) return;
    if (!bitflags.visible) return;
    if (color1.a == 0 && color2.a == 0) return;
    cnt++;
    /* GET PARENT VARIABLES */
    float px = 0, py = 0, pz = 0, prx = 0, pry = 0, prz = 0, psx = 1, psy = 1;
    if (parent != nullptr)
    {
        px = parent->pos.x+parent->entity_pos.x;
        py = -parent->pos.y-parent->entity_pos.y;
        pz = parent->pos.z+parent->entity_pos.z;
        prx = parent->rotation.x;
        pry = parent->rotation.y;
        prz = parent->rotation.z;
        psx = parent->scale.x;
        psy = parent->scale.y;
    }


    /* CALCULATE SCALE AND ANCHOR MULTIPLIERS AND COLORS */
    float XS = scale.x * scale_2.x * psx;
    float YS = scale.y * scale_2.y * psy;
    float l = bitflags.anchorX != 0 ? -(bitflags.anchorX - 1) : -0.5f;
    float r = l+1;
    float t = bitflags.anchorY != 0 ? (bitflags.anchorY - 1) : 0.5f;
    float b = t-1;
    NSEngine::Color c1 = color1;
    NSEngine::Color c2 = color2;

    if (!sb) sb = NSEngine::getLayer(layer);
    auto p = pos+pos2;
    NSEngine::draw_set_blend(bitflags.blendmode);
    //if (layer == 5) { p += glm::vec3(0,-192,0); }
    if (bitflags.originMode == 0)
    {
        auto windata = NSEngine::getInstance()->window().getWindowData();
        p += glm::vec3(-windata.bwidth/2,-windata.bheight/2,0);
    }
    //if (!(bitflags_hi & ANMVM_BIT_534_8)) { psx *=2; psy *=2; }
    /* MODE 3 : RECTANGLE      MODE 6 : RECTANGLE GRADIENT      MODE 12 : RECTANGLE BORDER*/
    if (mode_of_special_draw == 6 || mode_of_special_draw == 3 || mode_of_special_draw == 12)
    {
        glm::mat4 rotZ = glm::rotate(glm::mat4(1.0f), rotation.z, {0, 0, -1});
        float width = float_vars[2] * XS;
        float height = float_vars[3] * YS;
        glm::vec4 pp = glm::vec4(/*psx */ p.x + px, /*psy */ -p.y + py, p.z + pz, 0);
        glm::vec4 pos1 = pp + rotZ * glm::vec4(- width * l, - height * t, 0, 0);
        glm::vec4 pos2 = pp + rotZ * glm::vec4(+ width * r, - height * t, 0, 0);
        glm::vec4 pos3 = pp + rotZ * glm::vec4(+ width * r, + height * b, 0, 0);
        glm::vec4 pos4 = pp + rotZ * glm::vec4(- width * l, + height * b, 0, 0);
        c2 = mode_of_special_draw == 6 ? c2 : c1;
        batch_draw_quad_color_2d(sb, pos1, pos2, pos3, pos4, c1, c2, c2, c1);
        return;
    }

    /* MODE 4 : POLYGON      MODE 5 : POLYGON OUTLINE */
    if (mode_of_special_draw == 4 || mode_of_special_draw == 5)
    {
        float radius = float_vars[3] * scale.x * psx;
        int nbVert = int_vars[3];
        NSEngine::draw_circle_set_vertex_count(nbVert);
        if (mode_of_special_draw == 5) c2 = c1;
        batch_draw_circle_color(sb, p.x * psx + px, -p.y * psy + py, radius, c1, c2, mode_of_special_draw == 5);
        return;
    }

    /* MODE 13 : LINE */
    if (mode_of_special_draw == 13)
    {
        float len = float_vars[3] * XS;
        glm::mat4 rotate = glm::eulerAngleZYX(-rotation.z + prz, rotation.y + pry, rotation.x + prx);
        glm::vec4 pos2 = glm::vec4(pos,0) + rotate * glm::vec4(len, 0, 0, 0);
        draw_set_color(color1);
        batch_draw_line(sb, pos.x*psx - px, -pos.y*psy+py, pos2.x*psx+px, -pos2.y*psy+py, 1);
        draw_set_color(c_white);
        return;
    }

    /* MODE 11 : RING */
    if (mode_of_special_draw == 11)
    {
        NSEngine::draw_circle_set_vertex_count(int_vars[0]);
        draw_set_color(color1);
        float r1 = float_vars[3]*XS - float_vars[2]*YS/2.f;
        float r2 = float_vars[3]*XS + float_vars[2]*YS/2.f;
        batch_draw_circle_arc(sb, p.x+px, -p.y+py, r1, r2, 0, PI2);
        draw_set_color(c_white);
        return;
    }

    /* MODE 7 : RECT_ROT    8 : RECT_ROT_GRAD */
    if (mode_of_special_draw == 8 || mode_of_special_draw == 7)
    {
        float width = float_vars[2] * XS;
        float height = float_vars[3] * YS;
        c2 = mode_of_special_draw == 8 ? c2 : c1;
        //glm::mat4 rotate = glm::eulerAngleZYX(-rotation.z + prz, rotation.y + pry, rotation.x + prx);
        // TODO: rotate and fix position
        batch_draw_rectangle_color(sb, p.x+px-width*l,-p.y+py-height*t,p.x+px+width*r,-p.y+py+height*b,c1,c2,c2,c1);
        return;
    }

    /* GET SPRITE DATA */
    auto s = AnmManager::loadedFiles[anim_slot].getSprite(sprite_id);
    float u1 = (s.u1+uv_scroll_pos.x)*uv_scale.x;
    float u2 = (s.u2+uv_scroll_pos.x)*uv_scale.x;
    float v1 = (s.v1+uv_scroll_pos.y)*uv_scale.y;
    float v2 = (s.v2+uv_scroll_pos.y)*uv_scale.y;

    /* MODE 0,1,2 : ARCS */
    if (mode_of_special_draw == 1 || mode_of_special_draw == 2 || mode_of_special_draw == 0)
    {
        float r1 = scale.y-scale.x/2.f;
        float r2 = scale.y+scale.x/2.f;
        float angleStart = -rotation.z, angleEnd = angleStart+PI2;
        if (mode_of_special_draw == 1)
        {
            angleStart = -rotation.z - rotation.x/2.f;
            angleEnd = -rotation.z + rotation.x/2.f;
        }
        else if (mode_of_special_draw == 2)
        {
            angleEnd = angleStart+rotation.x;
        }
        NSEngine::draw_circle_set_vertex_count(int_vars[0]);
        draw_set_color(color1);
        //math::angle_normalize(angleStart);
        //math::angle_normalize(angleEnd);
        batch_draw_circle_arc_textured(sb, p.x*psx+px, -p.y*psy+py, r1, r2, angleStart, angleEnd, s.texID, u1, u2, int_vars[1]);
        draw_set_color(c_white);
        return;
    }

    /* MODE 10 : RING 3D */
    if (mode_of_special_draw == 10)
    {
        float r1 = float_vars[2] - float_vars[1]/2.f;
        float r2 = float_vars[2] + float_vars[1]/2.f;
        float a1 = float_vars[3] - float_vars[0]/2.f;
        float a2 = float_vars[3] + float_vars[0]/2.f;
        NSEngine::draw_circle_set_vertex_count(int_vars[0]);
        draw_set_color(color1);
        batch_draw_circle_arc_textured(sb, p.x+px, -p.y+py, r1, r2, a1, a2, s.texID, u1, u2, int_vars[1]);
        draw_set_color(c_white);
        return;
    }

    /* MODE 9 : CYLINDER */
    if (mode_of_special_draw == 9)
    {
        float r = float_vars[2];
        float h = float_vars[1];
        float a1 = float_vars[3] - float_vars[0]/2.f;
        float a2 = float_vars[3] + float_vars[0]/2.f;
        glm::vec3 posi = {p.x+px, -p.y+py, p.z+pz};
        glm::vec3 rota = rotation + glm::vec3(prx, pry, prz);
        NSEngine::draw_circle_set_vertex_count(int_vars[0]);
        draw_set_color(color1);
        batch_draw_cylinder(sb, posi, rota, r, h, a1, a2, s.texID, u1, u2, int_vars[1]);
        draw_set_color(c_white);
        return;
    }
    NSEngine::draw_set_blend(0);

    /* CALCULATE POSITION VECTORS */
    //glm::mat4 parentRotate = glm::eulerAngleYXZ(pry, prx, prz);
    //glm::vec4 pos4 = parentRotate * glm::vec4(p,0);
    glm::vec4 pos4 = glm::vec4(p,0);
    pos4.x *= psx;
    pos4.y *= -psy;

    glm::mat4 rotate;
    pos4 = glm::vec4(pos4.x+px+entity_pos.x,pos4.y+py-entity_pos.y,pos4.z+pz+entity_pos.z,0);

    if (bitflags.rendermode < 4) { pos4.z = 0; rotate = glm::rotate(glm::mat4(1.0f), -rotation.z + prz, glm::vec3(0,0,1)); }
    else rotate = glm::eulerAngleZYX(-rotation.z + prz, rotation.y + pry, -rotation.x - prx);
/*       if (layer == 3) {
        pos4.y += 2*224.f;
        XS*=2;
        YS*=2;
    } */

    if (bitflags.colmode == 1) c1 = c2;
    NSEngine::Color ctl = c1;
    NSEngine::Color ctr = c1;
    NSEngine::Color cbr = c1;
    NSEngine::Color cbl = c1;
    if (bitflags.colmode == 2) ctr = cbr = c2;
    if (bitflags.colmode == 3) cbl = cbr = c2;

    /* DRAW ON LAYER */
    NSEngine::Vertex tl = {{pos4 + rotate * glm::vec4(l * s.w * XS + anchor_offset.x, t * s.h * YS + anchor_offset.y, 0, 0)}, ctl, {u1, v1}};
    NSEngine::Vertex tr = {{pos4 + rotate * glm::vec4(r * s.w * XS + anchor_offset.x, t * s.h * YS + anchor_offset.y, 0, 0)}, ctr, {u2, v1}};
    NSEngine::Vertex br = {{pos4 + rotate * glm::vec4(r * s.w * XS + anchor_offset.x, b * s.h * YS + anchor_offset.y, 0, 0)}, cbr, {u2, v2}};
    NSEngine::Vertex bl = {{pos4 + rotate * glm::vec4(l * s.w * XS + anchor_offset.x, b * s.h * YS + anchor_offset.y, 0, 0)}, cbl, {u1, v2}};

    sb->draw(s.texID, tl, tr, br, bl, bitflags.blendmode);
}
