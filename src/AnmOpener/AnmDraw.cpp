#include "AnmFuncs.h"
#include "AnmManager.h"
#include "AnmVM.h"
#include <Engine.hpp>
#include <NSEngine.h>
#include <algorithm>
#include <glm/gtx/euler_angles.hpp>

void AnmVM::draw(NSEngine::SpriteBatch* sb) {
    if (index_of_on_draw)
        ANM_ON_DRAW_FUNCS[index_of_on_draw](this);

    /* CHECK IF THE VM IS VISIBLE */
    if (bitflags.activeFlags != ANMVM_ACTIVE && bitflags.activeFlags != ANMVM_FROZEN)
        return;
    if (!bitflags.visible || !bitflags.f530_1)
        return;
    if (color_1.a == 0 && color_2.a == 0)
        return;
    cnt++;
    /* GET PARENT VARIABLES */
    glm::vec3 parPos {}, parRot {};
    glm::vec2 parScal { 1, 1 };
    getParentData(parPos, parRot, parScal);
    float px = parPos.x, py = parPos.y, pz = parPos.z, prx = parRot.x, pry = parRot.y, prz = parRot.z, psx = parScal.x, psy = parScal.y;

    /* CALCULATE SCALE AND ANCHOR MULTIPLIERS AND COLORS */
    float XS = scale.x * scale_2.x * psx;
    float YS = scale.y * scale_2.y * psy;
    float l = bitflags.anchorX != 0 ? -(bitflags.anchorX - 1) : -0.5f;
    float r = l + 1;
    float t = bitflags.anchorY != 0 ? (bitflags.anchorY - 1) : 0.5f;
    float b = t - 1;
    NSEngine::Color c1 = color_1;
    if (parent_vm && bitflags.colorizeChildren) {
        if (parent_vm->bitflags.colmode == 1)
            c1 = NSEngine::mix(c1, parent_vm->color_2);
        else
            c1 = NSEngine::mix(c1, parent_vm->color_1);
    }

    NSEngine::Color c2 = color_2;

    if (!sb)
        sb = NSEngine::getLayer(layer);
    auto p = pos + __pos_2;
    auto s_ent_pos = entity_pos;
    NSEngine::draw_set_blend(bitflags.blendmode);
    // if (layer == 5) { p += glm::vec3(0,-192,0); }
    if (bitflags.resolutionMode == 2) {
        p /= 2;
        s_ent_pos /= 2;
        XS /= 2;
        YS /= 2;
    }
    if (bitflags.originMode == 0) {
        p += glm::vec3(-224, -16, 0);
    }
    // if (!(bitflags_hi & ANMVM_BIT_534_8)) { psx *=2; psy *=2; }
    /* MODE 16 : RECTANGLE      MODE 20 : RECTANGLE GRADIENT      MODE 27 : RECTANGLE BORDER*/
    /* MODE 21 : RECT_ROT    22 : RECT_ROT_GRAD */
    if (bitflags.rendermode == 20 || bitflags.rendermode == 16 || bitflags.rendermode == 27
        || bitflags.rendermode == 21 || bitflags.rendermode == 22) {
        glm::mat4 rotZ = glm::rotate(glm::mat4(1.0f), rotation.z, { 0, 0, -1 });
        float width = sprite_size.x * XS;
        float height = sprite_size.y * YS;
        c2 = bitflags.rendermode == 20 || bitflags.rendermode == 22 ? c2 : c1;
        //glm::vec4 pp = glm::vec4(/*psx */ p.x + px, /*psy */ -p.y + py, p.z + pz, 0);
        glm::vec4 pp = {get_own_transformed_pos(), 1};
        pp.y *= -1;
        if (bitflags.rendermode == 21 || bitflags.rendermode == 22) {
            glm::vec2 po1 = pp + rotZ * glm::vec4(width * l - anchor_offset.x-0.5, height * t + anchor_offset.y+0.5, 0, 0);
            glm::vec2 po2 = pp + rotZ * glm::vec4(width * r - anchor_offset.x+0.5, height * t + anchor_offset.y+0.5, 0, 0);
            glm::vec2 po3 = pp + rotZ * glm::vec4(width * r - anchor_offset.x+0.5, height * b + anchor_offset.y-0.5, 0, 0);
            glm::vec2 po4 = pp + rotZ * glm::vec4(width * l - anchor_offset.x-0.5, height * b + anchor_offset.y-0.5, 0, 0);
            auto c11 = c1; c11.a /= 2;
            auto c21 = c2; c21.a /= 2;
            batch_draw_quad_color_2d(sb, po1, po2, po3, po4, c11, c21, c21, c11);
        }
        glm::vec2 pos1 = pp + rotZ * glm::vec4(width * l - anchor_offset.x, height * t + anchor_offset.y, 0, 0);
        glm::vec2 pos2 = pp + rotZ * glm::vec4(width * r - anchor_offset.x, height * t + anchor_offset.y, 0, 0);
        glm::vec2 pos3 = pp + rotZ * glm::vec4(width * r - anchor_offset.x, height * b + anchor_offset.y, 0, 0);
        glm::vec2 pos4 = pp + rotZ * glm::vec4(width * l - anchor_offset.x, height * b + anchor_offset.y, 0, 0);
        batch_draw_quad_color_2d(sb, pos1, pos2, pos3, pos4, c1, c2, c2, c1, bitflags.rendermode == 27);
        return;
    }

    /* MODE 17 : POLYGON      MODE 18 : POLYGON OUTLINE */
    if (bitflags.rendermode == 17 || bitflags.rendermode == 18) {
        float radius = sprite_size.x * scale.x * psx;
        int nbVert = int_script_vars[0];
        NSEngine::draw_circle_set_vertex_count(nbVert);
        glm::vec4 pp = {get_own_transformed_pos(), 1};
        if (bitflags.rendermode == 18)
            c2 = c1;
        batch_draw_circle_color(sb, pp.x, -pp.y, radius, c1, c2, bitflags.rendermode == 18);
        return;
    }

    /* MODE 26 : LINE */
    if (bitflags.rendermode == 26) {
        float len = sprite_size.x * XS;
        glm::mat4 rotate = glm::eulerAngleZYX(-rotation.z + prz, rotation.y + pry, rotation.x + prx);
        glm::vec4 pos2 = glm::vec4(pos, 0) + rotate * glm::vec4(len, 0, 0, 0);
        draw_set_color(c1);
        batch_draw_line(sb, pos.x * psx - px, -pos.y * psy + py, pos2.x * psx + px, -pos2.y * psy + py, 1);
        draw_set_color(c_white);
        return;
    }

    /* MODE 19 : RING */
    if (bitflags.rendermode == 19) {
        NSEngine::draw_circle_set_vertex_count(int_script_vars[0]);
        draw_set_color(c1);
        float r1 = sprite_size.x * XS - sprite_size.y * YS / 2.f;
        float r2 = sprite_size.x * XS + sprite_size.y * YS / 2.f;
        batch_draw_circle_arc(sb, p.x + px, -p.y + py, r1, r2, 0, PI2);
        draw_set_color(c_white);
        return;
    }

    /* GET SPRITE DATA */
    auto s = AnmManager::loadedFiles[anm_loaded_index].getSprite(sprite_id);
    float u1 = (s.u1 + uv_scroll_pos.x) * uv_scale.x;
    float u2 = (s.u2 + uv_scroll_pos.x) * uv_scale.x;
    float v1 = (s.v1 + uv_scroll_pos.y) * uv_scale.y;
    float v2 = (s.v2 + uv_scroll_pos.y) * uv_scale.y;

    /* MODE 9,13,24 : ARCS */
    if (bitflags.rendermode == 9 || bitflags.rendermode == 13 || bitflags.rendermode == 14) {
        float r1 = YS - XS / 2.f;
        float r2 = YS + XS / 2.f;
        float angleStart = -rotation.z, angleEnd = angleStart + PI2;
        if (bitflags.rendermode == 13) {
            angleStart = -rotation.z - rotation.x / 2.f;
            angleEnd = -rotation.z + rotation.x / 2.f;
        } else if (bitflags.rendermode == 14) {
            angleEnd = angleStart - rotation.x;
        }
        NSEngine::draw_circle_set_vertex_count(int_script_vars[0]);
        draw_set_color(c1);
        // math::angle_normalize(angleStart);
        // math::angle_normalize(angleEnd);
        p.x = p.x * psx + s_ent_pos.x;
        p.y = p.y * psy + s_ent_pos.y;
        batch_draw_circle_arc_textured(sb, p.x + px, -p.y + py, r1, r2, angleStart, angleEnd, s.texID, u1, u2, int_script_vars[1]);
        draw_set_color(c_white);
        return;
    }

    /* MODE 10 : RING 3D */
    if (bitflags.rendermode == 25) {
        float r1 = float_script_vars[2] - float_script_vars[1] / 2.f;
        float r2 = float_script_vars[2] + float_script_vars[1] / 2.f;
        float a1 = float_script_vars[3] - float_script_vars[0] / 2.f;
        float a2 = float_script_vars[3] + float_script_vars[0] / 2.f;
        NSEngine::draw_circle_set_vertex_count(int_script_vars[0]);
        draw_set_color(c1);
        batch_draw_circle_arc_textured(sb, p.x + px, -p.y + py, r1, r2, a1, a2, s.texID, u1, u2, int_script_vars[1]);
        draw_set_color(c_white);
        return;
    }

    /* MODE 24 : CYLINDER */
    if (bitflags.rendermode == 24) {
        float rot = float_script_vars[2];
        float h = float_script_vars[1];
        float a1 = float_script_vars[3] - float_script_vars[0] / 2.f;
        float a2 = float_script_vars[3] + float_script_vars[0] / 2.f;
        glm::vec3 posi = pos + __pos_2 + entity_pos;
        posi.y *= -1;
        NSEngine::draw_circle_set_vertex_count(int_script_vars[0]);
        draw_set_color(c1);
        batch_draw_cylinder(sb, posi, rotation, rot, h, a1, a2, s.texID, u1, u2, int_script_vars[1]);
        draw_set_color(c_white);
        return;
    }
    
    if (bitflags.rendermode < 4) {
        glm::vec4 corners[] = {{}, {}, {}, {}};
        write_sprite_corners_2d(corners);

        if (bitflags.colmode == 1)
            c1 = c2;
        NSEngine::Color ctl = c1;
        NSEngine::Color ctr = c1;
        NSEngine::Color cbr = c1;
        NSEngine::Color cbl = c1;
        if (bitflags.colmode == 2)
            ctr = cbr = c2;
        if (bitflags.colmode == 3)
            cbl = cbr = c2;

        /* DRAW ON LAYER */
        NSEngine::Vertex tl = { corners[0], ctl, { u1, v1 } };
        NSEngine::Vertex tr = { corners[1], ctr, { u2, v1 } };
        NSEngine::Vertex bl = { corners[2], cbl, { u1, v2 } };
        NSEngine::Vertex br = { corners[3], cbr, { u2, v2 } };

        sb->draw(s.texID, tl, tr, br, bl, bitflags.blendmode);
        return;
    }

    if (bitflags.rendermode == 4 || bitflags.rendermode == 6) {
        write_sprite_corners__mode_4();
        if (bitflags.colmode == 1)
            c1 = c2;
        NSEngine::Color ctl = c1;
        NSEngine::Color ctr = c1;
        NSEngine::Color cbr = c1;
        NSEngine::Color cbl = c1;
        if (bitflags.colmode == 2)
            ctr = cbr = c2;
        if (bitflags.colmode == 3)
            cbl = cbr = c2;

        /* DRAW ON LAYER */
        NSEngine::Vertex tl = { SPRITE_TEMP_BUFFER[0].transformed_pos, ctl, { u1, v1 } };
        NSEngine::Vertex tr = { SPRITE_TEMP_BUFFER[1].transformed_pos, ctr, { u2, v1 } };
        NSEngine::Vertex bl = { SPRITE_TEMP_BUFFER[2].transformed_pos, cbl, { u1, v2 } };
        NSEngine::Vertex br = { SPRITE_TEMP_BUFFER[3].transformed_pos, cbr, { u2, v2 } };

        sb->draw(s.texID, tl, tr, br, bl, bitflags.blendmode);
        return;
    }

    if (bitflags.rendermode == 8 || bitflags.rendermode == 15) {
        // if 15 set fog
        // disable zwrite if set
        //__matrix_1 = glm::mat4(1.f);
        __matrix_2 = glm::mat4(1.f); // glm::scale(__matrix_1, {scale * scale_2, 1.f});
        // bitflags.scaled = false;
        if (bitflags.resolutionMode == 1) {
            __matrix_2[0][0] *= RESOLUTION_MULT;
            __matrix_2[1][1] *= RESOLUTION_MULT;
        } else if (bitflags.resolutionMode == 2) {
            __matrix_2[0][0] *= RESOLUTION_MULT * 0.5;
            __matrix_2[1][1] *= RESOLUTION_MULT * 0.5;
        }
        switch (bitflags.rotationMode) {
            case 0:
                if (rotation.z != 0.0) __matrix_2 *= glm::rotate(glm::mat4(1.f), rotation.z, {0, 0, -1});
                if (rotation.y != 0.0) __matrix_2 *= glm::rotate(glm::mat4(1.f), rotation.y, {0, 1, 0});
                if (rotation.x != 0.0) __matrix_2 *= glm::rotate(glm::mat4(1.f), rotation.x, {-1, 0, 0});
                break;
            case 1:
                if (rotation.y != 0.0) __matrix_2 *= glm::rotate(glm::mat4(1.f), rotation.y, {0, 1, 0});
                if (rotation.z != 0.0) __matrix_2 *= glm::rotate(glm::mat4(1.f), rotation.z, {0, 0, -1});
                if (rotation.x != 0.0) __matrix_2 *= glm::rotate(glm::mat4(1.f), rotation.x, {-1, 0, 0});
                break;
            case 2:
                if (rotation.z != 0.0) __matrix_2 *= glm::rotate(glm::mat4(1.f), rotation.z, {0, 0, -1});
                if (rotation.x != 0.0) __matrix_2 *= glm::rotate(glm::mat4(1.f), rotation.x, {-1, 0, 0});
                if (rotation.y != 0.0) __matrix_2 *= glm::rotate(glm::mat4(1.f), rotation.y, {0, 1, 0});
                break;
            case 3:
                if (rotation.x != 0.0) __matrix_2 *= glm::rotate(glm::mat4(1.f), rotation.x, {-1, 0, 0});
                if (rotation.z != 0.0) __matrix_2 *= glm::rotate(glm::mat4(1.f), rotation.z, {0, 0, -1});
                if (rotation.y != 0.0) __matrix_2 *= glm::rotate(glm::mat4(1.f), rotation.y, {0, 1, 0});
                break;
            case 4:
                if (rotation.y != 0.0) __matrix_2 *= glm::rotate(glm::mat4(1.f), rotation.y, {0, 1, 0});
                if (rotation.x != 0.0) __matrix_2 *= glm::rotate(glm::mat4(1.f), rotation.x, {-1, 0, 0});
                if (rotation.z != 0.0) __matrix_2 *= glm::rotate(glm::mat4(1.f), rotation.z, {0, 0, -1});
                break;
            case 5:
                if (rotation.x != 0.0) __matrix_2 *= glm::rotate(glm::mat4(1.f), rotation.x, {-1, 0, 0});
                if (rotation.y != 0.0) __matrix_2 *= glm::rotate(glm::mat4(1.f), rotation.y, {0, 1, 0});
                if (rotation.z != 0.0) __matrix_2 *= glm::rotate(glm::mat4(1.f), rotation.z, {0, 0, -1});
                break;
        }
        bitflags.rotated = false;
        glm::vec3 pp = {
            entity_pos.x + pos.x + __pos_2.x - anchor_offset.x * scale.x * scale_2.x,
            entity_pos.y + pos.y + __pos_2.y - anchor_offset.y * scale.y * scale_2.y,
            0.f
        };
        transform_coordinate(pp);
        pp.z = entity_pos.z + pos.z + __pos_2.z;
        __matrix_2[3][0] = pp.x;
        __matrix_2[3][1] = -pp.y;
        __matrix_2[3][2] = pp.z;
        auto col = color_1;
        if (bitflags.colmode == 1) {
          col = color_2;
        }
        if (AnmManager::use_custom_color_1c90a4c != 0) {
           col = NSEngine::mix(col, AnmManager::custom_color_1c90a48);
        }

        // todo: next

        float top = t * sprite_size.y * scale.y * scale_2.y;
        float bottom = b * sprite_size.y * scale.y * scale_2.y;
        float left = l * sprite_size.x * scale.x * scale_2.x;
        float right = r * sprite_size.x * scale.x * scale_2.x;

        NSEngine::Vertex tl = { { __matrix_2 * glm::vec4(left, top, 0, 1) }, col, { u1, v1 } };
        NSEngine::Vertex tr = { { __matrix_2 * glm::vec4(right, top, 0, 1) }, col, { u2, v1 } };
        NSEngine::Vertex br = { { __matrix_2 * glm::vec4(right, bottom, 0, 1) }, col, { u2, v2 } };
        NSEngine::Vertex bl = { { __matrix_2 * glm::vec4(left, bottom, 0, 1) }, col, { u1, v2 } };
        sb->draw(s.texID, tl, tr, br, bl, bitflags.blendmode);
        return;
    }

    NSEngine::draw_set_blend(0);

    /* CALCULATE POSITION VECTORS */
    // glm::mat4 parentRotate = glm::eulerAngleYXZ(pry, prx, prz);
    // glm::vec4 pos4 = parentRotate * glm::vec4(p,0);
    glm::vec4 pos4 = glm::vec4(p, 0);
    pos4.x *= psx;
    pos4.y *= -psy;

    glm::mat4 rotate;
    pos4 = glm::vec4(pos4.x + px + s_ent_pos.x, pos4.y + py - s_ent_pos.y, pos4.z + pz + s_ent_pos.z, 0);

    if (bitflags.rendermode < 4) {
        pos4.z = 0;
        rotate = bitflags.rendermode == 0 ? glm::mat4(1.f) : glm::rotate(glm::mat4(1.0f), -rotation.z + prz, glm::vec3(0, 0, 1));
    } else
        rotate = glm::eulerAngleZYX(-rotation.z + prz, rotation.y + pry, -rotation.x - prx);
    /*       if (layer == 3) {
            pos4.y += 2*224.f;
            XS*=2;
            YS*=2;
        } */

    if (bitflags.colmode == 1)
        c1 = c2;
    NSEngine::Color ctl = c1;
    NSEngine::Color ctr = c1;
    NSEngine::Color cbr = c1;
    NSEngine::Color cbl = c1;
    if (bitflags.colmode == 2)
        ctr = cbr = c2;
    if (bitflags.colmode == 3)
        cbl = cbr = c2;

    /* DRAW ON LAYER */
    NSEngine::Vertex tl = { { pos4 + rotate * glm::vec4(l * sprite_size.x * XS - anchor_offset.x, t * sprite_size.y * YS + anchor_offset.y, 0, 0) }, ctl, { u1, v1 } };
    NSEngine::Vertex tr = { { pos4 + rotate * glm::vec4(r * sprite_size.x * XS - anchor_offset.x, t * sprite_size.y * YS + anchor_offset.y, 0, 0) }, ctr, { u2, v1 } };
    NSEngine::Vertex br = { { pos4 + rotate * glm::vec4(r * sprite_size.x * XS - anchor_offset.x, b * sprite_size.y * YS + anchor_offset.y, 0, 0) }, cbr, { u2, v2 } };
    NSEngine::Vertex bl = { { pos4 + rotate * glm::vec4(l * sprite_size.x * XS - anchor_offset.x, b * sprite_size.y * YS + anchor_offset.y, 0, 0) }, cbl, { u1, v2 } };

    sb->draw(s.texID, tl, tr, br, bl, bitflags.blendmode);
}
