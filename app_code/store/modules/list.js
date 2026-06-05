import request from '@/utils/request.js'
const state={
	list:null
};
const mutations={
	DBLIST(state,value){
		console.log('车辆列表',value);
		state.list=value
	},
	UPDATELIST(state,value){
		console.log("余额数据",value);
		state.list.forEach(item=>{
			if(item.attributes.card_num===value.card_num){
				item.attributes.balance=value.money
				
				if(value.way===0){
					item.attributes.work_state=false
					item.attributes.balance-=2
				}
				else
				item.attributes.work_state=true
				
			}	
		})
		console.log(state.list[0].attributes);
	}
};
const actions={
	async dbList({commit}){
		await request({url:'workers'}).then(({data})=>{
			commit("DBLIST",data.data)
		})
	}
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