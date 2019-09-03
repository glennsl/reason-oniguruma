open TestFramework;

open Oniguruma;

describe("OnigRegExp", ({describe, _}) => {
  describe("allocation", ({test, _}) => {
    test("finalizer gets called for regexp", ({expect, _}) => {
      let regexp = OnigRegExp.create("\\w(\\d+)");
      let callCount = ref(0);
      Gc.finalise_last(() => incr(callCount), regexp);
      Gc.full_major();

      expect.int(callCount^).toBe(1);
    });
  });
  describe("create", ({test, _}) => {
    test("erronous regexp - never ending recursion", ({expect, _}) => {
      let r = OnigRegExp.create("?<abc>\\g<abc>");
      switch (r) {
      | Ok(_) => expect.int(0).toBe(1);
      | Error(msg) => expect.string(msg).toEqual("target of repeat operator is not specified");
      }
      
    });
    test("erronous regexp - undefined callout", ({expect, _}) => {
      let r = OnigRegExp.create("(*FOO)");
      switch (r) {
      | Ok(_) => expect.int(0).toBe(1);
      | Error(msg) => expect.string(msg).toEqual("target of repeat operator is not specified");
      }
      
    });
  });
  describe("search", ({test, _}) => {

    test("returns empty array if it does not match", ({ expect, _}) => {
      let r = OnigRegExp.create("\\w(\\d+)");
      switch (r) {
      | Error(_) => expect.string("Fail").toEqual("");
      | Ok(regex) => {
        let result = OnigRegExp.search("-----------", 0, regex);
        expect.int(Array.length(result)).toBe(0);
      }
      }
    });
    test("unicode character", ({expect, _}) => {
      let r = OnigRegExp.create("a");
      switch (r) {
      | Error(_) => expect.string("fail").toEqual(""); 
      | Ok(regex) =>
        let result = OnigRegExp.search("ç√Ωa", 0, regex);
        expect.int(Array.length(result)).toBe(1);
        expect.int(result[0].startPos).toBe(7);
        expect.string(result[0].match).toEqual("a");
      }

    });
    test("match unicode character", ({expect, _}) => {
      let r = OnigRegExp.create("√");
      switch (r) {
      | Error(_) => expect.string("fail").toEqual(""); 
      | Ok(regex) =>
        let result = OnigRegExp.search("ç√Ωa", 0, regex);
        expect.int(Array.length(result)).toBe(1);
        expect.int(result[0].startPos).toBe(2);
        expect.string(result[0].match).toEqual("√");
      }

    });
    test("returns regions if it does match", ({ expect, _}) => {
      let r = OnigRegExp.create("\\w(\\d+)");
      switch (r) {
      | Error(_) => expect.string("Fail").toEqual("");
      | Ok(regex) => {
        let result = OnigRegExp.search("----a123---", 0, regex);
        expect.int(Array.length(result)).toBe(2);
        expect.int(result[0].startPos).toBe(4);
        expect.int(result[0].endPos).toBe(8);
        expect.int(result[0].index).toBe(0);
        expect.string(result[0].match).toEqual("a123");
        expect.int(result[0].length).toBe(4);
        expect.int(result[1].startPos).toBe(5);
        expect.int(result[1].endPos).toBe(8);
        expect.int(result[1].index).toBe(1);
        expect.int(result[1].length).toBe(3);
        expect.string(result[1].match).toEqual("123");
      }
      }
    });
  });
});
