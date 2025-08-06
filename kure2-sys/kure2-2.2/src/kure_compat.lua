--[[
 kure_compat.lua

  Copyright (C) 2010 Stefan Bolus, University of Kiel, Germany

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
 
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
--]]

if kure == nil then 
   kure = {}
end
kure.compat = {}

-- The global context is named __c

function kure.compat.assert (name, R)
   -- Call the dedicated callback if the assertion fails.
   if (kure.is_empty(R)) and kure.assert_func ~= nil then 
      kure.assert_func (name)
   end
end

function kure.compat.O (R) return kure.O(kure.new(R)) end
function kure.compat.On1 (R) return kure.O(kure.new_n1(R)) end
function kure.compat.O1n (R) return kure.O(kure.new_1n(R)) end

function kure.compat.L (R) return kure.L(kure.new(R)) end
function kure.compat.Ln1 (R) return kure.L(kure.new_n1(R)) end
function kure.compat.L1n (R) return kure.L(kure.new_1n(R)) end

function kure.compat.I (R) return kure.I(kure.new(R)) end

function kure.compat.init (v) return kure.vec_begin(kure.new(v)) end

function kure.compat.TRUE() return kure.L(kure.new(__c)) end
function kure.compat.FALSE() return kure.O(kure.new(__c)) end

--[[ Converts a given Boolean expression into a 1x1 relation
representing the given value. ]]--
function bool_to_rel(b) 
   if b then return kure.compat.TRUE() else return kure.compat.FALSE() end end

function kure.compat.empty(R) return bool_to_rel (kure.is_empty(R)) end
function kure.compat.unival(R,S) return bool_to_rel (kure.is_univalent(R,S)) end
function kure.compat.eq(R,S) return bool_to_rel (kure.equals(R,S)) end
function kure.compat.incl(R,S) return bool_to_rel (kure.includes(R,S)) end

function kure.compat.randomperm (v)
   return kure.random_perm (kure.new_si(__c, kure.rel_get_rows_si(v), kure.rel_get_rows_si(v))) end

-- See src/parse.y why these closures are necessary.
function kure.compat.random (R, prob) 
   return kure.random (kure.new(R), prob) end
function kure.compat.random_closure(prob)
   return function (R) return kure.compat.random(R,prob) end end

-- Same as for kure.compat.random
function kure.compat.randomcf (R, prob)
   return kure.random_no_cycles (kure.new(R), prob) end
function kure.compat.randomcf_closure(prob)
   return function (R) return kure.compat.randomcf(R,prob) end end

function kure.compat.p_1 (dom) return kure.proj_1(kure.new(__c), dom) end;
function kure.compat.p_2 (dom) return kure.proj_2(kure.new(__c), dom) end;

function kure.compat.i_1 (dom) return kure.inj_1(kure.new(__c), dom) end;
function kure.compat.i_2 (dom) return kure.inj_2(kure.new(__c), dom) end;

function kure.compat.cardrel (vec) return kure.less_card (vec) end;
function kure.compat.cardfilter (pot,vec) 
   return kure.subsetvec_rel(pot,vec) end;

function kure.compat.cardeq(R,S) return bool_to_rel(kure.is_cardeq(R,S)) end
function kure.compat.cardlt(R,S) return bool_to_rel(kure.is_cardlt(R,S)) end
function kure.compat.cardleq(R,S) return bool_to_rel(kure.is_cardleq(R,S)) end
function kure.compat.cardgt(R,S) return kure.compat.cardlt(S,R) end
function kure.compat.cardgeq(R,S) return kure.compat.cardleq(S,R) end

function kure.compat.partial_funcs(R,S) 
   return kure.partial_funcs_si(kure.new(__c), kure.rel_get_rows_si(R), kure.rel_get_rows_si(S)) end
function kure.compat.total_funcs(R,S) 
   return kure.total_funcs_si(kure.new(__c), kure.rel_get_rows_si(R), kure.rel_get_rows_si(S)) end

-- 1-st and 2-nd for a given domain. We always return a new relation.
function kure.compat.comp1(dom) return kure.dom_comp1(__c,dom) end
function kure.compat.comp2(dom) return kure.dom_comp2(__c,dom) end