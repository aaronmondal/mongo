// Cannot implicitly shard accessed collections because of following errmsg: A single
// update/delete on a sharded collection must contain an exact match on _id or contain the shard
// key.
// @tags: [assumes_unsharded_collection]

let t = db.update_arraymatch1;
t.drop();

let o = {_id: 1, a: [{x: 1, y: 1}, {x: 2, y: 2}, {x: 3, y: 3}]};
t.insert(o);
assert.eq(o, t.findOne(), "A1");

let q = {"a.x": 2};
t.update(q, {$set: {b: 5}});
o.b = 5;
assert.eq(o, t.findOne(), "A2");

t.update({"a.x": 2}, {$inc: {"a.$.y": 1}});
o.a[1].y++;
assert.eq(o, t.findOne(), "A3");
