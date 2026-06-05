
//向数据库请求数据

import {APIURL} from './env'
function request(params){
	return new Promise((resolve,reject)=>{
		uni.request({
			...params,
			url:`http://${APIURL}:1337/api/`+params.url,
			success:(res)=>{
				resolve(res)
			},
			fail:(err)=>{
				reject(err)
			}
		})
	})
}
export default request