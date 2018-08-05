#ifndef HITABLE_LIST_HH
#define HITABLE_LIST_HH

#include "hitable.hh"

class hitable_list: public hitable {
public:
    hitable **list;
    int list_size;

    hitable_list() {}
    hitable_list(hitable **l, int n) { list = l; list_size = n; }

    virtual bool hit(const ray& r, float tmin, float tmax, hit_record& rec) const;
    virtual bool bounding_box(float t0, float t1, aabb &box) const;
};

bool hitable_list::hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
    hit_record temp_rec;
    bool hit_anything = false;
    float closest_so_far = t_max;
    for (int i = 0; i < list_size; ++i) {
        if (list[i]->hit(r, t_min, closest_so_far, temp_rec)) {
            hit_anything = true;
            closest_so_far = temp_rec.t;
            rec = temp_rec;
        }
    }
    return hit_anything;
}

bool hitable_list::bounding_box(float t0, float t1, aabb &box) const {
    if (list_size < 1) {
        return false;
    }
    aabb temp_box;
    bool first_true = list[0]->bounding_box(t0, t1, temp_box);
    if (!first_true) {
        return false;
    }
    box = temp_box;
    for (int i = 1; i < list_size; ++i) {
        if (list[0]->bounding_box(t0, t1, temp_box)) {
            box = surrounding_box(box, temp_box);
        } else {
            return false;
        }
    }
    return true;    
}

#endif /* HITABLE_LIST_HH */
