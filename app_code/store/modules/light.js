
// not use

const state={
	light:null,
	
};
const mutations={
	RECEIVELIGHT(state,value){
		console.log(value);
		state.light=value.led_lv
	}
};
const actions={

};
const getters={
	
}
export default {
  namespaced: true,
  state,
  getters,
  actions,
  mutations,
};