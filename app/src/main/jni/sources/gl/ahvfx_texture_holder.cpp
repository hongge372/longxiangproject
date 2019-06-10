//
//  ahvfx_texture_holder.cpp
//  AHVideoFXFramework
//
//  Created by longxiang on 2019/4/1.
//  Copyright © 2019 AH. All rights reserved.
//

#include "ahvfx_texture_holder.hpp"
#include "ahvfx_io.hpp"
#include "ahvfx_texture_duplicator.hpp"

struct ahvfx_texture_holder_node {
    ahvfx::texture_t texture;
    ahvfx_texture_holder_node *next = NULL;
    
    explicit ahvfx_texture_holder_node()
    : texture(ahvfx::texture_t())
    {
    }
    ~ahvfx_texture_holder_node() {
        ahvfx_log("delete holded texture:%d size:%d x %d", texture.id, texture.width(), texture.height());
        texture.SLT_DEL();
    }
};

class ahvfx_texture_holder_impl {
public:
    uint32_t capacity = 0, cur_size = 0, fIdx = 0, counter = 0, temp_counter = 0;
    ahvfx_texture_holder_node *head = NULL, *cur = NULL, *out_head = NULL;
    std::shared_ptr<ahvfx::texture_duplicator> duplicator;
public:
    explicit ahvfx_texture_holder_impl(uint32_t capacity) {
        if (capacity > 0) {
            this->capacity = capacity;
            init_buffer();
            lazy_load(duplicator, std::shared_ptr<ahvfx::texture_duplicator>(new ahvfx::texture_duplicator()));
        }
    }
    
    ~ahvfx_texture_holder_impl() {
        release_buffer();
    }
    
    void init_buffer() {
        ahvfx_texture_holder_node *previous = NULL;
        for (int i = 0; i < capacity; ++i) {
            ahvfx_texture_holder_node *node = new ahvfx_texture_holder_node();
            lazy_load(head, node);
            if (previous) {
                previous->next = node;
            }
            previous = node;
        }
        previous->next = head;
        cur = head;
        out_head = head;
    }
    
    void release_buffer() {
        int index = 0;
        do {
            if (head) {
                ahvfx_texture_holder_node *next = head->next;
                delete (head);
                head = next;
            }
        } while (++index < capacity);
    }
    
    
    void hold(ahvfx::texture_t in, uint32_t fIdx) {
        if (cur) {
            duplicator->copy(in, std::ref(cur->texture));
            cur = cur->next;
            cur_size++;
            if (cur_size > capacity) {
                cur_size = capacity;
            }
            counter++;
            this->fIdx = fIdx;
        }
    }
    
    const ahvfx::texture_t out() {
        
//        print_holder();
        
        if (cur_size && cur_size == capacity) {
            int n = counter % capacity;
            while (n--) {
                out_head = out_head->next;
            }
            ahvfx::texture_t rs = out_head->texture;
            out_head = head;
            return rs;
        }
        return ahvfx::texture_t();
    }
    
    void print_holder() {
        int index = 0;
        do {
            if (head) {
                ahvfx_texture_holder_node *next = head->next;
                ahvfx_log("texture: %d", head->texture.id);
                head = next;
            }
        } while (++index < capacity);
    }
    
    
    void begin_next() {
        temp_counter = counter;
    }
    void end_next() {
        counter = temp_counter;
    }
    const ahvfx::texture_t next_out() {
        counter += 1;
        return out();
    }
    bool full() const {
        return cur_size == capacity;
    }
};

using namespace ahvfx;

texture_holder::texture_holder(uint32_t capacity) {
    impl = std::shared_ptr<ahvfx_texture_holder_impl>(new ahvfx_texture_holder_impl(capacity));
}

texture_holder::~texture_holder() {
    
}

void texture_holder::hold(ahvfx::texture_t in, uint32_t fIdx) {
    impl->hold(in, fIdx);
}


const ahvfx::texture_t texture_holder::out() const {
    return impl->out();
}

void texture_holder::begin_next() {
    impl->begin_next();
}

void texture_holder::end_next() {
    impl->end_next();
}

const ahvfx::texture_t texture_holder::next_out() {
    return impl->next_out();
}

bool texture_holder::full() const {
    return impl->full();
}
