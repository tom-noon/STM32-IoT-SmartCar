
// not use

import request from '@/utils/request.js'
import {motor,buzzer} from "@/utils/mqttControl.js"
import store from '../../store'
import myAlert from '@/utils/myAlert.js'
import {
	doPublish
} from '@/utils/mqtt'
const state={ 
	newCard:null,
	cardNum:null
};
const mutations={
	//添加卡号
	ADDCARD(state,value){
		// console.log('人员注册');
	state.newCard=value.card_num
	},
	
	GETNUM(state,value){
		console.log(value);
		state.cardNum=value
	},
	UPDATENUM(state,value){
		state.cardNum=value
	},
};
const actions={
	// 获取已打卡的数量
	async getNum({commit},value){
		await request({url:'parkinglot'}).then(({data})=>{
			commit("GETNUM",data.usingNum)
		})
	},
	// 从数据库查询卡号是否存在,存在则开门,否则蜂鸣器报警
	async TAKECARD({commit,dispatch},value){
		console.log(value);
		if(value.way){
			// 入站
			// TODO:修改卡号状态成True
			console.log("入站")
			await request({url:`changeState/${value.card_num}`,method:'POST',data:{state:1,balance:value.money}}).then(({data})=>{
				if(data.code===200){
					store.commit('list/UPDATELIST',value)
					motor(1)
				}
			})	
		}else{
			// 出站
			// TODO: value中有卡号和余额如果
			let p=''
			await request({url:`getUser/${value.card_num}`}).then(({data})=>{
				p=data
			})
			if(p.worker.work_state)
			{
				await request({url:`changeState/${value.card_num}`,method:'POST',data:{state:0,balance:value.money-2}}).then(res=>{console.log(value.card_num,'出站了')} )
				store.commit('list/UPDATELIST',value)
				doPublish("testtopic/mode",JSON.stringify({motor:1,money:value.money-2}))
			}
			else{
				buzzer(1)
				myAlert("没有入站信息")
			}
			
		}
		
	},
	// 打开下班
	async LEAVE({commit}){
		await request({url:'minusPark'}).then(({data})=>{
			if(data.code===200)
			{
				commit("UPDATENUM",data.usingNum)
			}else{
				console.log("...");
			}
		})
	},
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